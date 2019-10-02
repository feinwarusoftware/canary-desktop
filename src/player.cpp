#include "player.h"
#include "bass.h"
#include "bassmix.h"

#include <QVector>

Player::Player(QObject* parent) : QObject(parent) {

}

HSTREAM mixer, source;
HSYNC sync;

int b; //current (or next?) song

bool isPlaying = FALSE; //stores if the player SHOULD be playing, not if it actually is (that is controlled by BASS itself)
				//e.g.: when you're dragging the playback bar, BASS is NOT playing (it's paused), but isPlaying = TRUE, so when you release the button it continues playing as intended

QVector<char const*> queue(10);

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

void Player::init() {
	BASS_PluginLoad("bassflac.dll", 0); //TODO: add more plugins
	//TODO: add notifier/throw error/error parsing
	BASS_Init(
		-1, //default device
		44100, //default freq of 44100, change it to the default device freq via flag
		BASS_DEVICE_FREQ,
		0,
		NULL
	);

	mixer = BASS_Mixer_StreamCreate(48000, 2, BASS_MIXER_END); //TODO GET FREQ
};

void Player::insertToQueue(int pos, char const* song) {
	queue.insert(pos, song);
}

bool Player::loadSong(int pos) {
	b = pos;
	sync = BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0);

	source = BASS_StreamCreateFile(FALSE, queue[pos], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
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
	isPlaying = TRUE;
	return BASS_ChannelPlay(mixer, FALSE);
}

bool Player::pause() {
	isPlaying = FALSE;
	return BASS_ChannelPause(mixer);
}

bool Player::changeVolume(float v) {
	return BASS_SetVolume(v);
}

bool Player::active() {
	return isPlaying;
}

/*
TODO FUNCTIONS:
- (automatically) Change output device when main device changes
- remove songs from queue
- get freq in init (mixer init)
*/