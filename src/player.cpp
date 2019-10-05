#include "player.h"
#include "bass.h"
#include "bassmix.h"

#include <QVector>
#include <QString>
#include <QTimer>

Player::Player(QObject* parent) : QObject(parent) {

}

HSTREAM mixer, source;
HSYNC sync;

int b; //current (or next?) song

QWORD sLen, cLen;

QObject* root;

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

QVector<char const*> queue(10);

QTimer* timer;

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	if (b + 1 > queue.size() - 1) {
		return;
	};
	b = b + 1;
	source = BASS_StreamCreateFile(FALSE, queue[b], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open next source
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

	mixer = BASS_Mixer_StreamCreate(48000, 2, BASS_MIXER_END); //TODO GET FREQ

	timer = new QTimer(this);
	timer->setInterval(500);
	connect(timer, &QTimer::timeout, this, [=]() {
		setCurrentLen(0);
	});
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
	timer->start();
	return BASS_ChannelPlay(mixer, FALSE);
}

bool Player::pause() {
	timer->stop();
	isPlaying = FALSE;
	return BASS_ChannelPause(mixer);
}

bool Player::changeVolume(float v) {
	return BASS_ChannelSetAttribute(mixer, BASS_ATTRIB_VOL, v);
}

bool Player::active() {
	return isPlaying;
}

/*QWORD Player::getLength(HSTREAM src) { //not being used right now - may be useful in the future
	return BASS_ChannelGetLength(src, BASS_POS_BYTE);
}*/

void Player::setLen() {
	sLen = BASS_ChannelGetLength(source, BASS_POS_BYTE); //implicit conversion from QWORD to int
	//root->findChild<QObject*>("debug")->setProperty("text", sLen);
	root->findChild<QObject*>("trackbar")->setProperty("to", sLen);
	root->findChild<QObject*>("staticCounter")->setProperty("text", toMinHourFormat(sLen));
}

void Player::setCurrentLen(bool drag) { //if being dragged or called by timeout
	cLen = BASS_ChannelGetPosition(
		source,
		BASS_POS_BYTE
	);
	root->findChild<QObject*>("dynamicCounter")->setProperty("text", toMinHourFormat(cLen));
	if (!drag) {
		root->findChild<QObject*>("trackbar")->setProperty("value", cLen);
	}
}

bool Player::seek(int to) {
	timer->stop();
	if (isPlaying) {
		BASS_ChannelPause(mixer);
	}
	BASS_ChannelSetPosition(source, to, BASS_POS_BYTE);
	return BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
}

QWORD Player::getPosition() {
	return BASS_ChannelGetPosition(source, BASS_POS_BYTE);
}

int seconds, minutes, remaining;
QString Player::toMinHourFormat(QWORD bytes) { //it's always based on the "source" stream so no need to pass that
	seconds = BASS_ChannelBytes2Seconds(source, bytes);
	minutes = seconds / 60;
	remaining = seconds - minutes * 60;
	if (remaining >= 10) {
		return QString::number(minutes) + ":" + QString::number(remaining);
	}
	else {
		return QString::number(minutes) + ":0" + QString::number(remaining); //maybe TODO: make the "optional" 0 a variable (it's a stretch but idk hahaha)
	}
}

/*
TODO FUNCTIONS:
- (automatically) Change output device when main device changes
- remove songs from queue
- get freq in init (mixer init)
- fix array
*/

// BASS_ChannelGetLength(activeChannel, BASS_POS_BYTE)