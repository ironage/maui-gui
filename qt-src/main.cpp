#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "mcvplayer.h"
#include "mremoteinterface.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MCVPlayer>("com.maui.custom", 1, 0, "MCVPlayer");
    qmlRegisterType<MPoint>("com.maui.custom", 1, 0, "MPoint");
    qmlRegisterType<MLogMetaData>("com.maui.custom", 1, 0, "MLogMetaData");
    qmlRegisterType<MRemoteInterface>("com.maui.custom", 1, 0, "MRemoteInterface");
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    app.setQuitOnLastWindowClosed(true);
    return app.exec();
}
