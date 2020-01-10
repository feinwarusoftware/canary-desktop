#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QWebEngineView>
#include <QtWebChannel/QtWebChannel>
#include <QObject>
#include <QString>

#include "player.h"

#include "library.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine("qrc:/main.qml");

    Player player;
	Library library;

	QScopedPointer<Player> p(new Player);
    engine.rootContext()->setContextProperty("player", p.data());

	QScopedPointer<Library> l(new Library);
	engine.rootContext()->setContextProperty("library", l.data());

    player.init(engine.rootObjects().first());

	player.insertToQueue(0, "Ok Goodnight - Think Again.flac");
    //player.insertToQueue(1, "Ok Goodnight - Think Again.flac");
	//player.insertToQueue(0, "G:/Músicas/Avenged Sevenfold/City of Evil/04 - Bat Country.flac");

	//library.searchDir(QString::fromUtf8(u8"G:/Músicas"));

	//library.createLib();

    //player.loadSong(0);

    QWebEngineView view;
    return app.exec();
}
