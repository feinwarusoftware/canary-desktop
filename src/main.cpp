#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include "player.h"
#include <QVector>
//#include <QString>

#include "bass.h"
#include "bassmix.h"

#include <iostream>

QObject* debugger;

//QVector<char const*> queue(10);

//VOID PLAYSOLOSONG CLEAR QUEUE E ADD SONG E PLAY SONG [0]

int main(int argc, char* argv[])
{
	Player player;

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine("main.qml");

	QScopedPointer<Player> p(new Player);

	engine.rootContext()->setContextProperty("player", p.data());

	//debugger = engine.rootObjects().first()->findChild<QObject*>("debug");

	player.init();

	//obviously test songs
	player.insertToQueue(0, "03. Lisbon.flac");

	player.loadSong(0);

	/*QQuickView* view = new QQuickView;
	view->setSource(QUrl::fromLocalFile("main.qml"));
	view->show(); //this is debug?*/

	//player.play();

	return app.exec();
}