#include <QApplication>
#include <QQmlApplicationEngine>
#include "myplayer.h"
#include "kvconfig2.h"

KVConfig2 *_cfg = 0;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    _cfg = new KVConfig2("student_detect_trace.config");

    qmlRegisterType<MyPlayer>("zonekey.qd", 1, 3, "Player");
    qmlRegisterType<KVConfig2>("zonekey.qd", 1, 3, "KVConfig");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
