#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include <QObject>
#include <QVector>
#include <QVariant> //TODO: replace for GUI or shit later on? ===> doing it progressively

class Player : public QObject {
    Q_OBJECT
public:
    explicit Player(QObject* parent = 0);
    void init(QObject* r);
    void insertToQueue(int pos, char const* song);
	Q_INVOKABLE bool loadSong(int pos);
    Q_INVOKABLE double getPositionInSeconds();
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool changeVolume(float v);
	Q_INVOKABLE bool seek(double to);
	Q_INVOKABLE bool jump(bool direction);

private:
	int getCSLengthInSeconds();
	bool loadPlugins();

signals:

public slots:
};

#endif //PLAYER_H
