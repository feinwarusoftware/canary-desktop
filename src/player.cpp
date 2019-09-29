#include "player.h"
#include "bass.h"
#include "bassmix.h"

#include <QVector>

HSTREAM mixer, source;
HSYNC sync;

int b; //current (or next?) song

QVector<char const*> queue(10);

void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	//if (index > 0 && index < myVector.size()). ===== O CONTRÁRIO
	if (b + 1 > queue.size() - 1) { // NÃO TEM PRÓXIMO
		//debugger->setProperty("text", "oi");
		return;
	}; //TODO AQUI -> AQUELA HORA DEU BUG PQ ISSO AQUI SEMPRE É VERDADE, LOGO ELE SEMPRE RETORNAVA E TOCAVA SPREAD YOUR FIRE
	b = b + 1;
	source = BASS_StreamCreateFile(FALSE, queue[b], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT); // open next source
	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN); // plug it in
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE); // reset the mixer
	//no need to recall the callback, it is called every time the mixer "ends" (it's attached to the mixer)
}

void Player::init() {
	//BASS_PluginLoad("bassflac.dll", 0); //TODO: add more plugins
	//TODO: add notifier/throw error/error parsing
	BASS_Init(
		-1, //default device
		44100, //default freq of 44100, change it to the default device freq via flag
		BASS_DEVICE_FREQ,
		0,
		NULL
	);

	mixer = BASS_Mixer_StreamCreate(48000, 2, BASS_MIXER_END); // add if here / same freq as the sys - BASS wil "downgrade" if needed / get it to init? (got)
};

void Player::insertToQueue(int pos, char const* song) {
	queue.insert(pos, song);
}

bool Player::loadSong(int pos) {
	b = pos;
	//mixer = BASS_Mixer_StreamCreate(48000, 2, BASS_MIXER_END); // add if here / same freq as the sys - BASS wil "downgrade" if needed / get it to init?
	sync = BASS_ChannelSetSync(mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, EndSync, 0);

	source = BASS_StreamCreateFile(FALSE, queue[pos], 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	BASS_Mixer_StreamAddChannel(mixer, source, BASS_STREAM_AUTOFREE);

	return true;
}

bool Player::clearMixer() {
	BASS_Mixer_ChannelRemove(source);
	return BASS_ChannelRemoveSync(mixer, sync);
}

bool Player::skipSong() {
	clearMixer();
	loadSong(b + 1);
	BASS_ChannelSetPosition(mixer, 0, BASS_POS_BYTE);
	return true;
}

bool Player::backSong() {
	return true;
}

bool Player::play() {
	BASS_ChannelPlay(mixer, FALSE);
	return true;
}

/*bool Player::playSong(QString song) {
	BASS_Mixer_StreamCreate(44100, 2, BASS_MIXER_END);
}*/

/*
TODO FUNCTIONS:
- (automatically) Change output device when main device changes
*/