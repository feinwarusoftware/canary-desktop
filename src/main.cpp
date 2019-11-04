#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVector>
#include <QQuickImageProvider>
//#include <QString>

#include "bass.h"
#include "bassmix.h"

#include <iostream>

#include "player.h"

QObject *debugger, *trackbar;

bool hasLoadedSong = false; //used on first init or when queue finishes

int main(int argc, char* argv[])
{
	Player player;

	CCover coveryay;

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine("main.qml");

	QScopedPointer<Player> p(new Player);

	engine.rootContext()->setContextProperty("player", p.data());

	debugger = engine.rootObjects().first()->findChild<QObject*>("debug");
	trackbar = engine.rootObjects().first()->findChild<QObject*>("trackbar");

	player.init(engine.rootObjects().first()); //maybe this passing of the root object won't be necessary in the future (functions will be called directly from QML already with the necessary, specific objects)

	/*TagLib::FileRef f("03. Lisbon.flac");
	QImage co("tumblr_pke584kKYX1s0v7b0o5_400.png");
	//coveryay.getCover(f, co);

	bool save = co.save("image.png");

	engine.rootObjects().first()->findChild<QObject*>("debug")->setProperty("text", save);*/

	//obviously test songs
	//player.insertToQueue(0, "03. Lisbon.flac");
	/*player.insertToQueue(0, "01. Deus Le Volt!.flac");
	player.insertToQueue(1, "02. Spread Your Fire.flac");
	player.insertToQueue(2, "03. Angels And Demons.flac");*/
	player.insertToQueue(0, "03. Lisbon.flac");
	player.insertToQueue(1, "G:/Músicas/KARMA/2006 - Leave Now!!!/02. Crawl.flac");
	player.insertToQueue(2, "Ok Goodnight - Think Again.flac");
	player.insertToQueue(3, "G:/Músicas/Stream of Passion/Embrace The Storm/03. Deceiver.flac");

	if (!hasLoadedSong) {
		player.loadSong(0);
	}

	/*QQuickView* view = new QQuickView;
	view->setSource(QUrl::fromLocalFile("main.qml"));
	view->show(); //this is debug?*/

	//player.play();

	return app.exec();
}