#ifndef MYUDPSOCKET_H
#define MYUDPSOCKET_H

#include <QQuickItem>
#include <QUdpSocket>

// 指定host, port, 发送数据 ..
class MyUdpSocket : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString targetHost READ targetHost WRITE setTargetHost NOTIFY targetHostChanged)
    Q_PROPERTY(int targetPort READ targetPort WRITE setTargetPort NOTIFY targetPortChanged)

public:
    MyUdpSocket();

    QString targetHost() const { return target_host_; }
    void setTargetHost(const QString &host) { target_host_ = host; }

    int targetPort() const { return target_port_; }
    void setTargetPort(int port) { target_port_ = port; }

    // 发送 09 01
    Q_INVOKABLE int send_cmd_restart()
    {
        return send_bytes("\0x090x01", 2);
    }

    // 发送 08 01
    Q_INVOKABLE int send_cmd_start()
    {
        return send_bytes("\0x080x01", 2);
    }

    Q_INVOKABLE int send_cmd_stop()
    {
        return send_bytes("\0x080x03", 2);
    }

signals:
    void targetHostChanged();
    void targetPortChanged();

public slots:

private:
    int send_bytes(const char *data, int len)
    {
        QUdpSocket sock;
        QByteArray ba(data, len);
        QHostAddress remote;
        remote.setAddress(target_host_);    // 必须是ip
        sock.writeDatagram(ba, remote, target_port_);

        return 0;
    }

    QString target_host_;
    int target_port_;
};

#endif // MYUDPSOCKET_H
