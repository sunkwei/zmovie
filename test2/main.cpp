#include <QApplication>
#include <QQmlApplicationEngine>
#include "myplayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<MyPlayer>("zonekey.qd", 1, 1, "Player");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
