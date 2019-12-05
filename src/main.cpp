#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QWebEngineView>
#include <QtWebChannel/QtWebChannel>
#include <QObject>

#include "player.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine("qrc:/main.qml");

    Player player;

    QScopedPointer<Player> p(new Player);
    engine.rootContext()->setContextProperty("player", p.data());

    player.init(engine.rootObjects().first());

	player.insertToQueue(0, "01 - The Triumph.mp3");
    player.insertToQueue(1, "Ok Goodnight - Think Again.flac");

    //player.loadSong(0);

    QWebEngineView view;
    return app.exec();
}
