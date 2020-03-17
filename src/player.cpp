#include "player.h"

#ifdef _WIN32
#define PLATFORM 0

#elif __linux__
#define PLATFORM 1

#elif __APPLE__
#define PLATFORM 2

#endif

BASS_INFO currentDeviceInfo;

HSTREAM mixer, source;
HSYNC sync, timeSync;

int b; //variable used as position controller

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

bool lastCallWasPrev = false;

bool shuffle = false, ignoreShuffle = false;

int repeat = 0;

float volume = 1;

struct songStruct {
	QByteArray dir;
	QJSValue data;
	bool isPlayingNow;
};

QVector<songStruct> queue;

QWORD syncpos;

std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution<int> distr;

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
	Player player;

	if (!lastCallWasPrev) { // "previous" call
		queue[b].isPlayingNow = false;
	}
	else {
		lastCallWasPrev = false;
	}

	/*priorizes repeat 2 over shuffle, shuffle over repeat 1*/
	if (!shuffle && repeat != 2 && b + 1 > queue.size() - 1) {
		if (repeat == 1) {
			qDebug() << "pular para o primeiro";
			player.jumpTo(0);
			return;
		}
		qDebug() << "cabou negada";
		player.clearQueue();
		return;
	};

	if (shuffle && queue.size() > 0 && repeat != 2 && !ignoreShuffle) {
		qDebug() << shuffle;
		b = distr(eng);
	}

	else if (!shuffle && repeat != 2) {
		b = b + 1;
	}

	if (ignoreShuffle) {
		ignoreShuffle = false;
	}

	source = BASS_StreamCreateFile(FALSE, queue[b].dir.toStdString().c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open 1st source
	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN); // plug it in
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); // reset the mixer

	syncpos = BASS_ChannelSeconds2Bytes(source, 0.5);
	timeSync = BASS_ChannelSetSync(source, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	queue[b].isPlayingNow = true;

	QMetaObject::invokeMethod(root, "changeNowPlaying",
		Q_ARG(QVariant, queue[b].data.toVariant())
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

void Player::insertToQueue(int pos, QJSValue data){
	QByteArray fileName = QFile::encodeName(data.property("dir").toString()); //pre-converts the directory to prevent playback rampin from taking too long

	songStruct songObj;

	songObj.dir = fileName;
	songObj.data = data;
	songObj.isPlayingNow = false;

	distr = std::uniform_int_distribution<int>(0, queue.size());

	queue.insert(pos, songObj);
}

bool Player::loadSong(int pos) {
	b = pos;

	qDebug() << queue[pos].dir;

	source = BASS_StreamCreateFile(FALSE,
		queue[pos].dir.toStdString().c_str(),  //there seems to be no issues in using a pointer here - the value of "source" can change and it doesn't affect playback
		0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	bool addToMixer = BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);
	bool resetMixer = BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);

	syncpos = BASS_ChannelSeconds2Bytes(source, 0.5);
	timeSync = BASS_ChannelSetSync(source, BASS_SYNC_POS | BASS_SYNC_MIXTIME | BASS_SYNC_ONETIME, syncpos, TimerSync, 0);

	queue[b].isPlayingNow = true;

	QMetaObject::invokeMethod(root, "changeNowPlaying",
		Q_ARG(QVariant, queue[pos].data.toVariant())
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
	if (!isPlaying) { //verifying in case isPlaying is already TRUE
		isPlaying = TRUE;
	}

	emit updatePlaying(true);

	if (BASS_ChannelGetPosition(source, BASS_POS_BYTE) == 0) { //TODO? remove this
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

	emit updatePlaying(false);

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
		if (shuffle) {
			b = distr(eng);
		}
		else if (repeat == 2) {
			b = b + 1;
		}
		
		//if (!shuffle) {
			return BASS_Mixer_ChannelRemove(source) /*when this happens, the next song is automatically called*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
		//}
	}
	else {
		if (b > 0) {
			queue[b].isPlayingNow = false;
			lastCallWasPrev = true;
			if (shuffle) {
				b = distr(eng);
			}
			else if (repeat == 2) {
				b = b - 1;
			}
			else {
				b = b - 2;
			}

			return BASS_Mixer_ChannelRemove(source) /*like trick with the "b - 2" thing - this makes the EndSync callback load  the "next of the previous of the previous" => -2 + 1 = -1*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
		}
		else {
			clearQueue();
			bool remove = BASS_Mixer_ChannelRemove(source);
			bool reset = BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
			return remove && reset;
		}
	}

	return false;
}

bool Player::jumpTo(int pos) {
	if (!isPlaying) {
		play();
	}

	queue[b].isPlayingNow = false;

	if (repeat == 2 && pos == 0) {
		b = 0;
	}
	else if (repeat == 2 || shuffle) {
		b = pos;
	}
	else {
		b = pos - 1;
	}

	if (pos == 0) {
		lastCallWasPrev = true;
	}

	if (shuffle) {
		ignoreShuffle = true;
	}

	return BASS_Mixer_ChannelRemove(source) /*when this happens, the next song is automatically called*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);

}

void Player::clearQueue() {
	isPlaying = FALSE;
	queue.clear();
	QMetaObject::invokeMethod(root, "clear");
}

bool Player::playing() {
	return isPlaying;
}

bool Player::resetQueue() {
	clearQueue();
	bool remove, reset, wasShuffle = false, wasRepeat = false;

	if (shuffle) {
		qDebug() << "tem shuffle!";
		shuffle = false;
		wasShuffle = true;
	}

	if (repeat == 2) {
		repeat = 0;
		wasRepeat = true;
	}

	remove = BASS_Mixer_ChannelRemove(source);
	reset = BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);

	if (wasShuffle) {
		shuffle = true;
	}

	if (wasRepeat) {
		repeat = 2;
	}

	return remove && reset;
}

QVariantList Player::getQueue() {
	QVariantList q;

	for (songStruct& s : queue) {
		QVariantMap d = s.data.toVariant().toMap();
		d.insert("isPlayingNow", s.isPlayingNow);
		q.append(d);
	}

	return q;
}

void Player::setRepeat(int n) {
	repeat = n;
}

bool Player::setShuffle(bool to) {
	shuffle = to;
	return shuffle;
}

bool Player::getShuffle() {
	return shuffle;
}

/*
TODO NOTE: Weird crash when loading test song, jumping (playback finished) the loading an album. I should take a closer look.
*/