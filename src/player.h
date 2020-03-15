#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include "covers.h"

#include <QObject>

#include <QVariant>
#include <QString>
#include <QFile>
#include <QFileInfo>

#include <QMap>
#include <QVariantMap>

#include <QVector>
#include <QList>

#include <QJSValue>

#include <random>

#include <QDebug>

class Player : public QObject {
    Q_OBJECT
public:
    explicit Player(QObject* parent = 0);
    void init(QObject* r);
    Q_INVOKABLE void insertToQueue(int pos, QJSValue data);
    Q_INVOKABLE bool loadSong(int pos);
    Q_INVOKABLE double getPositionInSeconds();
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool changeVolume(float v);
    Q_INVOKABLE bool seek(double to);
    Q_INVOKABLE bool jump(bool direction);
    Q_INVOKABLE bool jumpTo(int pos);
    Q_INVOKABLE void clearQueue();
    Q_INVOKABLE bool playing();
    Q_INVOKABLE QVariantList getQueue();
    Q_INVOKABLE bool resetQueue();
    Q_INVOKABLE void setRepeat(int n);
    Q_INVOKABLE bool setShuffle(bool to);
    Q_INVOKABLE bool getShuffle();

private:
    bool loadPlugins();

signals:
    void updatePlaying(bool to);
};

#endif //PLAYER_H