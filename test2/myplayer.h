#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>

class MyPlayer : public QQuickPaintedItem
{
    Q_OBJECT

public:
    MyPlayer();
    virtual void paint(QPainter *painter);
};

#endif // MYPLAYER_H
