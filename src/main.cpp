#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QtWebEngine>
#include <QtWebEngineCore>
#include <QWebEngineView>

#include <QObject>

#include "player.h"
#include "library.h"

int main(int argc, char* argv[])
{
    QtWebEngine::initialize();
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Player player;
    Library library;
    engine.rootContext()->setContextProperty("player", &player); //exposes the C++ classes to QML
    engine.rootContext()->setContextProperty("library", &library);

    //QObject::connect(player, &Player::loadSong, someFunction);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml"))); //loads the QML interface

    /*QObject* root = engine.rootObjects().first();

    QObject::connect(
        &library, &Library::libLoaded,
        [=](QVariantList& l) {
            qDebug() << l;
        }
    );*/

    player.init(engine.rootObjects().first()); //loads BASS, it's plugins, set app render frequency based on the system, etc.

    /*
        acknowledgment: I know passing the QML root as a pointer so the class can deal with it is a terrible workaround - I should be using pointers. thing is, they don't work properly
        because I'm obligated to deal with scoped functions with BASS, or static functions

        if you find a workaround using signals, feel free to make a pull request!
    */

    return app.exec();
}