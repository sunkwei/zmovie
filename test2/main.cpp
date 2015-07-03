#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include "myplayer.h"
#include "kvconfig2.h"
#include "utils.h"
#include "ptz.h"
#include "myudpsocket.h"

KVConfig2 *_cfg = 0;

int main(int argc, char *argv[])
{
    log_init();

    QApplication app(argc, argv);

    QString locale = QLocale::system().name();
    QTranslator trans;
    bool rc = trans.load(QString("test2-") + locale);
    if (rc) {
        app.installTranslator(&trans);
    }

    qmlRegisterType<MyPlayer>("zonekey.qd", 1, 4, "Player");
    qmlRegisterType<KVConfig2>("zonekey.qd", 1, 4, "KVConfig");
    qmlRegisterType<Ptz>("zonekey.qd", 1, 4, "Ptz");
    qmlRegisterType<MyUdpSocket>("zonekey.qd", 1, 5, "UdpSocket");
    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/MyMain.qml")));
    //engine.load(QUrl(QStringLiteral("./MyMain.qml")));

    return app.exec();
}
