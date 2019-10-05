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

QObject *debugger, *trackbar;

//QVector<char const*> queue(10);

//VOID PLAYSOLOSONG CLEAR QUEUE E ADD SONG E PLAY SONG [0]

int main(int argc, char* argv[])
{
	Player player;

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine("main.qml");

	QScopedPointer<Player> p(new Player);

	engine.rootContext()->setContextProperty("player", p.data());

	debugger = engine.rootObjects().first()->findChild<QObject*>("debug");
	trackbar = engine.rootObjects().first()->findChild<QObject*>("trackbar");

	player.init(engine.rootObjects().first()); //maybe this passing of the root object won't be necessary in the future (functions will be called directly from QML already with the necessary, specific objects)

	//obviously test songs
	player.insertToQueue(0, "03. Lisbon.flac");

	player.loadSong(0);

	/*QQuickView* view = new QQuickView;
	view->setSource(QUrl::fromLocalFile("main.qml"));
	view->show(); //this is debug?*/

	//player.play();

	return app.exec();
}