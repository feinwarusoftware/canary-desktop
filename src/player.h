#ifndef PLAYER_H
#define PLAYER_H

#include "bass.h"
#include "bassmix.h"

#include <QVector>
#include <QString>
#include <QTimer>

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
	Q_INVOKABLE bool pause();
	//Q_INVOKABLE QWORD getLength(HSTREAM src);
	void setLen();
	Q_INVOKABLE bool seek(int to);
	Q_INVOKABLE QWORD getPosition();
	QString toMinHourFormat(QWORD bytes);
	Q_INVOKABLE void setCurrentLen(bool drag = 0);


signals:

public slots:
};

#endif //PLAYER_H