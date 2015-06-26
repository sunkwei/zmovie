#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include "myplayer.h"
#include "kvconfig2.h"
#include "utils.h"
#include "ptz.h"

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

    _cfg = new KVConfig2("student_detect_trace.config");

    qmlRegisterType<MyPlayer>("zonekey.qd", 1, 4, "Player");
    qmlRegisterType<KVConfig2>("zonekey.qd", 1, 4, "KVConfig");
    qmlRegisterType<Ptz>("zonekey.qd", 1, 4, "Ptz");
    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/MyMain.qml")));

    return app.exec();
}
