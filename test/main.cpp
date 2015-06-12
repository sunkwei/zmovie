#include <QApplication>
#include <QQmlApplicationEngine>
#include "piechart.h"
#include "player.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<PieChart>("PieChart", 1, 0, "PieChart");
    qmlRegisterType<Player>("Player", 1, 0, "Player");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
