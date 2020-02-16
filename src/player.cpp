#include "player.h"

#ifdef _WIN32
#define PLATFORM 0

#elif __linux__
#define PLATFORM 1

#elif __APPLE__
#define PLATFORM 2

#endif

CCover cover;

BASS_INFO currentDeviceInfo;

HSTREAM mixer, source;
HSYNC sync, timeSync;

int b; //variable used as position controller

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

float volume = 1;

struct songStruct {
	QByteArray dir;
	QMap<QString, QVariant> data;
};

QVector<songStruct> queue;
//std::vector<char const*>::iterator queueIterator;

QWORD syncpos;

QObject* root;

Player::Player(QObject* parent) : QObject(parent) {

}

void CALLBACK TimerSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	Player player;

	QMetaObject::invokeMethod(
		root,
		"changeTime",
		Q_ARG(QVariant, player.getPositionInSeconds())
	);

	syncpos += BASS_ChannelSeconds2Bytes(channel, 0.5);
	timeSync = BASS_ChannelSetSync(channel, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);
}
bool Player::loadPlugins() {
	/*
		WMA playback not supported - would be Windows exclusive and it's not widely used anyways - I, personally, never used it in years
		MIDI plyaback not supported - didn't want to deal with the whole soundfont stuff - sorry for the fellow old-school soundtrackers!
		SPX and MPC not supported cause... does anybody use this... at all... for music, at least?
	*/

	if (PLATFORM == 0) { //is WINDOWS
		//MAYBE TODO: redo this based on prefixes (lib) and sufixes (.dll, .so, .dylib, etc.)
		return
			BASS_PluginLoad("bassflac.dll", 0)
			&& BASS_PluginLoad("basswv.dll", 0)
			&& BASS_PluginLoad("bassopus.dll", 0)
			&& BASS_PluginLoad("bassalac.dll", 0)
			&& BASS_PluginLoad("bass_ape.dll", 0) //TODO: MP4 tupport(?)
			&& BASS_PluginLoad("bass_aac.dll", 0)
			&& BASS_PluginLoad("bass_ac3.dll", 0)
			&& BASS_PluginLoad("bass_tta.dll", 0);
		//&& BASS_PluginLoad("basshls.dll", 0) //FUTURE FEATURE, when podcasts are supported
	}
	else if (PLATFORM == 1) { //is Linux
		return
			BASS_PluginLoad("libbassflac.so", 0)
			&& BASS_PluginLoad("libbasswv.so", 0)
			&& BASS_PluginLoad("libbassopus.so", 0)
			&& BASS_PluginLoad("libbassalac.so", 0)
			&& BASS_PluginLoad("libbass_ape.so", 0)
			&& BASS_PluginLoad("libbass_aac.so", 0)
			&& BASS_PluginLoad("libbass_ac3.so", 0)
			&& BASS_PluginLoad("libbass_tta.so", 0);
	}
	else if (PLATFORM == 2) {
		return //some of the formats are natively supported by Mac, so plugins are not necessary
			BASS_PluginLoad("libbassflac.dylib", 0)
			&& BASS_PluginLoad("libbasswv.dylib", 0)
			&& BASS_PluginLoad("libbassopus.dylib", 0)
			&& BASS_PluginLoad("libbass_ape.dylib", 0)
			&& BASS_PluginLoad("libbass_ac3.dylib", 0)
			&& BASS_PluginLoad("libbass_tta.dylib", 0);
	}
}

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	if (b + 1 > queue.size() - 1) {
		qDebug() << "cabou negada";
		Player player;
		player.clearQueue();
		return;
	};

	b = b + 1;
	
	source = BASS_StreamCreateFile(FALSE, queue[b].dir.toStdString().c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open 1st source
	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN); // plug it in
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); // reset the mixer

	syncpos = BASS_ChannelSeconds2Bytes(source, 0.5);
	timeSync = BASS_ChannelSetSync(source, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	QMetaObject::invokeMethod(root, "changeNowPlaying",
		Q_ARG(QVariant, QVariantMap(queue[b].data))
	);
}

void Player::init(QObject* r) {
	root = r;

	//queueIterator = queue.begin();

	BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1); //change output device on the fly

	loadPlugins();

	//TODO: add notifier/throw error/error parsing
	BASS_Init(
		-1, //default device
		44100, //default freq of 44100, change it to the default device freq via flag
		BASS_DEVICE_FREQ,
		0,
		NULL
	);

	BASS_GetInfo(&currentDeviceInfo);
	mixer = BASS_Mixer_StreamCreate(currentDeviceInfo.freq, currentDeviceInfo.speakers, BASS_MIXER_END | BASS_MIXER_RESUME);
	BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0); // set sync for end
}

void Player::insertToQueue(int pos, QString song) {
	QByteArray fileName = QFile::encodeName(song);

	TagLib::FileRef file(fileName.toStdString().c_str());

	QImage CScoverArt; //preparing variable to recieve cover art

	cover.getCover(file, CScoverArt, 200, 200, QFileInfo(fileName.toStdString().c_str()).canonicalPath());

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	CScoverArt.save(&buffer, "JPG", 100);

	QString uri = "data:image/jpg;base64," + QString::fromLatin1(ba.toBase64().data()); //converting to Base64 buffer

	QString artist = QString::fromWCharArray(file.tag()->artist().toWString().c_str());
	QString album = QString::fromWCharArray(file.tag()->album().toWString().c_str());
	QString title = QString::fromWCharArray(file.tag()->title().toWString().c_str());

	QString length = QString::number(file.audioProperties()->length());

	songStruct songObj;

	songObj.dir = fileName;

	songObj.data.insert("title", title);
	songObj.data.insert("artist", artist);
	songObj.data.insert("album", album);
	songObj.data.insert("coverUri", uri);
	songObj.data.insert("len", length);

	queue.insert(pos, songObj);
}

/*
	source = BASS_StreamCreateFile(FALSE, queue[b].dir.toStdString().c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open 1st source
	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN); // plug it in
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); // reset the mixer

	syncpos = BASS_ChannelSeconds2Bytes(source, 0.5);
	timeSync = BASS_ChannelSetSync(source, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	QMetaObject::invokeMethod(root, "changeNowPlaying",
		Q_ARG(QVariant, QVariantMap(queue[b].data))
	);
*/

bool Player::loadSong(int pos) {
	b = pos;

	qDebug() << queue[pos].dir.toStdString().c_str();

	source = BASS_StreamCreateFile(FALSE, 
		queue[pos].dir.toStdString().c_str(),  //there seems to be no issues in using a pointer here - the value of "source" can change and it doesn't affect playback
		0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	bool addToMixer = BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);
	bool resetMixer = BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);

	syncpos = BASS_ChannelSeconds2Bytes(source, 0.5);
	timeSync = BASS_ChannelSetSync(source, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	QMetaObject::invokeMethod(root, "changeNowPlaying",
		Q_ARG(QVariant, QVariantMap(queue[pos].data))
	);

	if (!isPlaying) {
		play();
	}

	//put song in mixer bool
	return addToMixer && resetMixer;
}

//changed from int to double for precision
double Player::getPositionInSeconds() {
	return BASS_ChannelBytes2Seconds(source, BASS_ChannelGetPosition(source, BASS_POS_BYTE));
}

bool Player::play() {
	if (!isPlaying) { //verifying in case isPlaying is already TRUE (~~this happens while the trackbar is being dragged, for example, as a play function is called when it's released~~ not anymore)
		isPlaying = TRUE;
	}

	if (BASS_ChannelGetPosition(source, BASS_POS_BYTE) == 0) {
		return BASS_ChannelPlay(mixer, FALSE);
	}

	return BASS_ChannelPlay(mixer, FALSE) && BASS_ChannelSlideAttribute(mixer, BASS_ATTRIB_VOL, volume, 250);
}

bool Player::pause() {
	isPlaying = FALSE;

	BASS_ChannelSlideAttribute(
		mixer,
		BASS_ATTRIB_VOL,
		0,
		250
	);	
	return BASS_ChannelPause(mixer);
}

bool Player::changeVolume(float v) {
	volume = v;
	return BASS_ChannelSetAttribute(mixer, BASS_ATTRIB_VOL, volume);
}

bool Player::seek(double to) {
	QWORD toInBytes = BASS_ChannelSeconds2Bytes(source, to);

	bool fadeOut = BASS_ChannelSlideAttribute(mixer, BASS_ATTRIB_VOL, 0, 100); //fade out for 100 miliseconds
	bool seekToPosition = BASS_ChannelSetPosition(source, toInBytes, BASS_POS_BYTE);
	bool resetMixer = BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
	bool fadeIn = BASS_ChannelSlideAttribute(mixer, BASS_ATTRIB_VOL, volume, 100);

	syncpos = BASS_ChannelSeconds2Bytes(mixer, 0.5);
	timeSync = BASS_ChannelSetSync(mixer, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	/*syncpos = BASS_ChannelSeconds2Bytes(mixer, 0.5);
	timeSync = BASS_ChannelSetSync(mixer, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);*/

	return fadeOut && seekToPosition && resetMixer && fadeIn;
}

bool Player::jump(bool direction) {
	//if (!active()) {
	if (!isPlaying) {
		play();
	}

	if (direction) {
		return BASS_Mixer_ChannelRemove(source) /*when this happens, the next song is automatically called*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
	}
	else {
		if (b > 0) {
			b = b - 2;

			return BASS_Mixer_ChannelRemove(source) /*like trick with the "b - 2" thing - this makes the EndSync callback load  the "next of the previous of the previous" => -2 + 1 = -1*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
		}
		else {
			clearQueue();
			return true;
		}
	}

	return false;
}

void Player::clearQueue() {
	isPlaying = FALSE;
	queue.clear();
	queue.squeeze(); //frees allocated memory places - maybe not necessary
	QMetaObject::invokeMethod(root, "clear");
	qDebug() << volume;
}

bool Player::playing() {
	return isPlaying;
}

QVariantList Player::getQueue() {
	QVariantList q;

	for (songStruct& s : queue) {
		qDebug() << s.data["title"];
		q.append(QVariantMap(s.data));
	}

	return q;
}