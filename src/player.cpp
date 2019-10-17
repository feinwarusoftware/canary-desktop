#include "player.h"
#include "bass.h"
#include "bassmix.h"

#include <QVector>
#include <QString>

Player::Player(QObject* parent) : QObject(parent) {

}

HSTREAM mixer, source;
HSYNC sync;

int b; //current (or next?) song

float volume = 1;

QWORD sLen/*, cLen*/;

QObject* root;

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

QVector<char const*> queue; //apparently this works so fine (I got a glitch once but apparently it was unrelated to this and just a coincidence)

BASS_INFO currentDeviceInfo;

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	Player player;

	if (b + 1 > queue.size() - 1) {
		return;
	};

	b = b + 1;
	source = BASS_StreamCreateFile(FALSE, queue[b], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open next source

	player.setLen();

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
	mixer = BASS_Mixer_StreamCreate(currentDeviceInfo.freq, 2, BASS_MIXER_END); //TODO GET FREQ
};

void Player::insertToQueue(int pos, char const* song) {
	queue.insert(pos, song);
}

bool Player::loadSong(int pos) {
	b = pos;
	sync = BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0);

	source = BASS_StreamCreateFile(FALSE, queue[pos], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

	setLen();

	return BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);
}

bool Player::clearMixer() {
	BASS_Mixer_ChannelRemove(source);
	return BASS_ChannelRemoveSync(mixer, sync);
}

bool Player::jump(int direction) {
	if (direction == -1 && b - 1 < 0) {
		BASS_ChannelSetPosition(source, 0, BASS_POS_BYTE);
		return BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); //reset the mixer aswell to prevent audio glitching TODO: ADD FADE (TO ALL OF THIS STUFF)
	} 
	else if (direction == 1 && b + 1 > queue.size() - 1) {
		return clearMixer();
	};
	clearMixer();
	loadSong(b + direction); //1 if jump, -1 if back
	return BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
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

/*QWORD Player::getLength() { //not being used right now - may be useful in the future
	return BASS_ChannelGetLength(source, BASS_POS_BYTE);
}*/

void Player::setLen() {
	sLen = BASS_ChannelGetLength(source, BASS_POS_BYTE); //implicit conversion from QWORD to int

	QMetaObject::invokeMethod(root, "setLength", Q_ARG(QVariant, sLen));
}

/*void Player::setCurrentLen(QWORD pos, bool drag) { //if being dragged or called by timeout
	root->findChild<QObject*>("dynamicCounter")->setProperty("text", toMinHourFormat(pos));
	if (!drag) {
		root->findChild<QObject*>("trackbar")->setProperty("value", pos);
	}
}*/

int seekVar;
bool Player::seek(int to, int width) {
	seekVar = (to * sLen) / width;
	if (seekVar == sLen) {
		BASS_ChannelSetPosition(source, seekVar - 1, BASS_POS_BYTE); //-1 to actually work - if the "entire" value is passed it actually glitches and goes back to where (the position) it was before
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

int seconds, minutes, remaining;
QString Player::toMinHourFormat(int bytes) { //it's always based on the "source" stream so no need to pass that
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

void Player::updateTime(QWORD newpos) {
	root->findChild<QObject*>("dynamicCounter")->setProperty("text", toMinHourFormat(newpos));
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