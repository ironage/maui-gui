#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "mmediaplayer.h"
#include "mcvplayer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MMediaPlayer>("com.maui.custom", 1, 0, "MMediaPlayer");
    qmlRegisterType<MMediaPlayer>("com.maui.custom", 1, 0, "MOCVSource");
    qmlRegisterType<MCVPlayer>("com.maui.custom", 1, 0, "MCVPlayer");
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    app.setQuitOnLastWindowClosed(true);
    return app.exec();
}
