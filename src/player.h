#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include <QVector>

#include <QObject>
#include <QVariant> //TODO: replace for GUI or shit later on?

class Player : public QObject {
	Q_OBJECT
public:
	explicit Player(QObject* parent = 0);
	void init();
	Q_INVOKABLE void insertToQueue(int pos, char const* song);
	Q_INVOKABLE bool loadSong(int pos);
	bool clearMixer();
	Q_INVOKABLE bool jump(int direction);
	Q_INVOKABLE bool play();

signals:

public slots:
};

#endif //PLAYER_H