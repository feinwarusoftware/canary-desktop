#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"
#include <QVector>
//#include <QString>

#include "bass.h"
#include "bassmix.h"

QObject* debugger;

//QVector<char const*> queue(10);

//VOID PLAYSOLOSONG CLEAR QUEUE E ADD SONG E PLAY SONG [0]

int main(int argc, char* argv[])
{
	Player player;

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine("main.qml");

	debugger = engine.rootObjects().first()->findChild<QObject*>("debug");

	BASS_PluginLoad("bassflac.dll", 0);
	player.init();

	//obviously test songs
	player.insertToQueue(0, "01. Deus Le Volt!.flac");
	player.insertToQueue(1, "02. Spread Your Fire.flac");
	player.insertToQueue(3, "03. Angels And Demons.flac");

	player.loadSong(1);

	player.play();

	return app.exec();
}