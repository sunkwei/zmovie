#include <QApplication>
#include <QQmlApplicationEngine>
extern "C" {
#   include <libavformat/avformat.h>
}

int main(int argc, char *argv[])
{
    av_register_all();

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
