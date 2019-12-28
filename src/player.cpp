#include "player.h"

#include <QDebug>

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
HSYNC sync;

int b; //variable used as position controller

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

float volume = 1;

QVector<QString> queue;

QObject* root;

Player::Player(QObject* parent) : QObject(parent) {

}

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	Player player;

	if (b + 1 > queue.size() - 1) {
		return queue.clear();
	};

	b = b + 1;

    player.loadSong(b);

	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
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

void Player::init(QObject* r){
	root = r; //expose the QML engine to the player as a global variable

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
	mixer = BASS_Mixer_StreamCreate(currentDeviceInfo.freq, currentDeviceInfo.speakers, BASS_MIXER_END);
    sync = BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0);
}

//void Player::insertToQueue(int pos, char const* song) {
void Player::insertToQueue(int pos, QString song) {
	//qDebug() << song.toStdString().c_str();

	/*QByteArray fileName = QFile::encodeName(song);
	char const* encodedFileDir = fileName.constData();*/

	//qDebug() << encodedFileDir;

	queue.insert(pos, song);

	qDebug() << queue;
}

int Player::getCSLengthInSeconds(){
	return BASS_ChannelBytes2Seconds(source, BASS_ChannelGetLength(source, BASS_POS_BYTE));
}

bool Player::loadSong(int pos){
	b = pos;

	QByteArray fileName = QFile::encodeName(queue[pos]); //same code from library
	const char* encodedFileDir = fileName.constData();

	source = BASS_StreamCreateFile(FALSE, encodedFileDir, 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

	bool addToMixer = BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);

	//GET CURRENT SONG METADATA
	TagLib::FileRef file(encodedFileDir);

	QImage CScoverArt; //preparing variable to recieve cover art

	cover.getCover(file, CScoverArt, 200, 200);

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	CScoverArt.save(&buffer, "JPG", 100);

	QString uri = "data:image/jpg;base64," + QString::fromLatin1(ba.toBase64().data()); //converting to Base64 buffer

	QMetaObject::invokeMethod(root, "updateCSInfo", Q_ARG(QVariant, getCSLengthInSeconds()), Q_ARG(QVariant, uri), Q_ARG(QVariant, file.tag()->artist().toCString()), Q_ARG(QVariant, file.tag()->album().toCString()), Q_ARG(QVariant, file.tag()->title().toCString())); //CALL QML->JS WITH THE DATA
	
	//PUT SONG IN MIXER
	return addToMixer;
}

/*int Player::getPosition() { //in Q_INVOKABLE functions, always CONVERT QWORD TO INT so it's QML READABLE
	return BASS_ChannelGetPosition(source, BASS_POS_BYTE);
}*/

//changed from int to double for precision
/*int*/ double Player::getPositionInSeconds() { //in Q_INVOKABLE functions, always CONVERT QWORD TO INT so it's QML READABLE
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

	return fadeOut && seekToPosition && resetMixer && fadeIn;
}

bool Player::jump(bool direction) {
	//if (!active()) {
	if(!isPlaying){
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
			return pause(); //TEMPORARY - WILL STOP QUEUE IN THE FUTURE (just like mbee)
		}
	}

	return false;
}

//TODO: remove drag from pause