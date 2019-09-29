#include "bass.h"
#include "bassmix.h"
//#include <QString>

#include <QVector>

class Player {
public:
	void init();
	void insertToQueue(int pos, char const* song);
	bool loadSong(int pos);
	bool clearMixer();
	bool skipSong();
	bool backSong();
	bool play();
	//bool playSong(QString song);
};