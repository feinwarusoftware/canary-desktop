#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include <QVector>
#include <QString>

#include <vector>

#include <QObject>
#include <QVariant> //TODO: replace for GUI or shit later on?

class Player : public QObject {
	Q_OBJECT
public:
	explicit Player(QObject* parent = 0);
	void init(QObject* r);
	Q_INVOKABLE void insertToQueue(int pos, char const* song);
	Q_INVOKABLE bool loadSong(int pos);
	bool clearMixer();
	Q_INVOKABLE bool jump(int direction);
	Q_INVOKABLE bool play();
	Q_INVOKABLE bool changeVolume(float v);
	Q_INVOKABLE bool active();
	Q_INVOKABLE bool pause(bool drag = 0);
	Q_INVOKABLE QWORD getLength();
	void setLen();
	Q_INVOKABLE bool seek(int to, int width);
	Q_INVOKABLE int getPosition();
	//QString toMinHourFormat(QWORD bytes);
	Q_INVOKABLE QString toMinHourFormat(int bytes);
	//Q_INVOKABLE void setCurrentLen(QWORD pos, bool drag = 0);
	//Q_INVOKABLE int getRelativeLength(int len, int width);
	Q_INVOKABLE void updateTime(QWORD newpos);
	Q_INVOKABLE int bytesToSeconds(int bytes);


signals:

public slots:
};

#endif //PLAYER_H