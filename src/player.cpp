#include "player.h"

Player::Player(QObject* parent) : QObject(parent) {

}

HSTREAM mixer, source;
HSYNC sync;

int b; //current (or next?) song

float volume = 1;

QWORD sLen;

QObject* root;

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

QVector<char const*> queue; //apparently this works so fine (I got a glitch once but apparently it was unrelated to this and just a coincidence)

BASS_INFO currentDeviceInfo;

CCover cover;

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{

	root->findChild<QObject*>("debug")->setProperty("text", "fui chamado");


	Player player;

	if (b + 1 > queue.size() - 1) {
		return;
	};

	b = b + 1;
	source = BASS_StreamCreateFile(FALSE, queue[b], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open next source

	//TODO: USE LOADSONG HERE!!!

	player.setLen();

	player.setInfo(queue[b]);

	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN); // plug it in
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); // reset the mixer
	//no need to recall the callback, it is called every time the mixer "ends" (it's attached to the mixer)
}

void Player::init(QObject* r) {
	BASS_PluginLoad("bassflac.dll", 0); //TODO: add more plugins
	//TODO: add notifier/throw error/error parsing
	BASS_Init(
		-1, //default device
		44100, //default freq of 44100, change it to the default device freq via flag
		BASS_DEVICE_FREQ,
		0,
		NULL
	);

	root = r;

	BASS_GetInfo(&currentDeviceInfo);
	mixer = BASS_Mixer_StreamCreate(currentDeviceInfo.freq, 2, BASS_MIXER_END);
};

void Player::insertToQueue(int pos, char const* song) {
	queue.insert(pos, song);
}

bool Player::loadSong(int pos) {
	b = pos;
	sync = BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0);

	source = BASS_StreamCreateFile(FALSE, queue[pos], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

	setLen();

	setInfo(queue[pos]);

	return BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);
}

bool Player::clearMixer() {
	BASS_Mixer_ChannelRemove(source);
	return BASS_ChannelRemoveSync(mixer, sync);
}

//THIS IS HELLA BROKEN!!!
bool Player::jump(int direction) {
	
	if (direction == 1) {
		return BASS_Mixer_ChannelRemove(source) /*when this happens, the next song is automatically called*/ && BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
	}


	return true;
}

bool Player::play() {
	if (!isPlaying) { //verifying in case isPlaying is already TRUE (this happens while the trackbar is being dragged, for example, as a play function is called when it's released)
		isPlaying = TRUE;
	}
	QMetaObject::invokeMethod(root, "timerControl", Q_ARG(QVariant, 1));
	if (getPosition() == 0) {
		return BASS_ChannelPlay(mixer, FALSE);
	}
	return BASS_ChannelPlay(mixer, FALSE) && BASS_ChannelSlideAttribute(mixer, BASS_ATTRIB_VOL, volume, 100);
}

bool Player::pause(bool drag) {
	if (!drag) {
		isPlaying = FALSE;
	}
	BASS_ChannelSlideAttribute(
		mixer,
		BASS_ATTRIB_VOL,
		0,
		100
	);
	QMetaObject::invokeMethod(root, "timerControl", Q_ARG(QVariant, 0));
	return BASS_ChannelPause(mixer);
}

bool Player::changeVolume(float v) {
	volume = v;
	return BASS_ChannelSetAttribute(mixer, BASS_ATTRIB_VOL, volume);
}

bool Player::active() {
	return isPlaying;
}

void Player::setLen() {
	sLen = BASS_ChannelGetLength(source, BASS_POS_BYTE); //implicit conversion from QWORD to int

	QMetaObject::invokeMethod(root, "setLength", Q_ARG(QVariant, sLen));
}

bool Player::seek(int to, int width) {
	int seekVar;
	seekVar = (to * sLen) / width;
	if (seekVar == sLen) {
		BASS_ChannelSetPosition(source, seekVar - BASS_ChannelSeconds2Bytes(source, 1), BASS_POS_BYTE); //-1 to actually work - if the "entire" value is passed it actually glitches and goes back to where (the position) it was before
		//TODO: arithmetic overflow on the previous line???
		return BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
	}
	BASS_ChannelSetPosition(source, seekVar, BASS_POS_BYTE);
	return BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
}

int Player::getPosition() { //in Q_INVOKABLE functions, always CONVERT QWORD TO INT so it's QML READABLE
	return BASS_ChannelGetPosition(source, BASS_POS_BYTE);
}

int Player::bytesToSeconds(int bytes) {
	return BASS_ChannelBytes2Seconds(source, bytes);
}

QString Player::toMinHourFormat(int bytes) { //it's always based on the "source" stream so no need to pass that
	int seconds, minutes, remaining;
	seconds = bytesToSeconds(bytes);
	minutes = seconds / 60;
	remaining = seconds - minutes * 60;
	if (remaining >= 10) {
		return QString::number(minutes) + ":" + QString::number(remaining);
	}
	else {
		return QString::number(minutes) + ":0" + QString::number(remaining); //maybe TODO: make the "optional" 0 a variable (it's a stretch but idk hahaha)
	}
}

/*struct songData {
	QString title;
	QString artist;
	QString album;
	QImage coverArt;
};*/

#include <QFile>
#include <QTextStream>

bool Player::setInfo(const char* songDir) {
	TagLib::FileRef file(songDir); //TODO: create a wider, class based tag retriever that will also be used in library scan, then use it here

	bool c = cover.getCover(file, root->findChild<QObject*>("coverArt"));

	bool d = QMetaObject::invokeMethod(root, "reload");

	return QMetaObject::invokeMethod(root, "setCurrentSongData", 
		Q_ARG(QVariant, file.tag()->title().toCString()),
		Q_ARG(QVariant, file.tag()->album().toCString()),
		Q_ARG(QVariant, file.tag()->artist().toCString())
	) && c && d;
}

/*
TODO FUNCTIONS:
- (automatically) Change output device when main device changes
- remove songs from queue
- get freq in init (mixer init) DONE
- fix array
- actually do something when queue ends
- WAVEFORM: calculate each pixels byte value and jump from/by those bytes and drawing them (their peaks)
- remove unused libraries
*/