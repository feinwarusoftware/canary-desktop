#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include "covers.h"

#include <QObject>

#include <list>

#include <QVariant>
#include <QString>
#include <QFile>
#include <QFileInfo>

#include <QDebug>

class Player : public QObject {
    Q_OBJECT
public:
    explicit Player(QObject* parent = 0);
    void init(QObject* r);
    Q_INVOKABLE void insertToQueue(int pos, QString song);
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
};

#endif //PLAYER_H