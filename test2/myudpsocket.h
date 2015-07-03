#ifndef MYUDPSOCKET_H
#define MYUDPSOCKET_H

#include <QQuickItem>
#include <QUdpSocket>

// 指定host, port, 发送数据 ..
class MyUdpSocket : public QQuickItem
{
    Q_OBJECT

public:
    MyUdpSocket();

signals:

public slots:
};

#endif // MYUDPSOCKET_H
