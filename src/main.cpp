#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QtWebEngine>
#include <QtWebEngineCore>
#include <QWebEngineView>

#include "player.h"

int main(int argc, char* argv[])
{
    QtWebEngine::initialize();
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Player player;
    engine.rootContext()->setContextProperty("player", &player); //exposes the C++ class to QML

    engine.load(QUrl(QStringLiteral("qrc:/main.qml"))); //loads the QML interface

    player.init(engine.rootObjects().first()); //loads BASS, it's plugins, set app render frequency based on the system, etc.
    /*
        acknowledgment: I know passing the QML root as a pointer so the class can deal with it is a terrible workaround - I should be using pointers. thing is, they don't work properly
        because I'm obligated to deal with scoped functions with BASS, or static functions

        if you find a workaround using signals, feel free to make a pull request!
    */

    player.insertToQueue(0, "testsongs/(01) [Akira Yamaoka] One More Soul To The Call.flac");

    return app.exec();
}