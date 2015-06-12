#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QQuickItem>
#include "zkrender.h"

class Player : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl)

public:
    Player();

    void setUrl(const QString &url);
    QString url() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();


private:
    QString url_;
    zkrender_t *render_;
};

#endif // PLAYER_H
