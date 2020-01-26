#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QtWebEngine>
#include <QtWebEngineCore>
#include <QWebEngineView>

int main(int argc, char* argv[])
{
    QtWebEngine::initialize();
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}