#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>

class PieChart : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE seturl)

public:
    PieChart();
    void seturl(const QString &url);
    QString url() const;

    void paint(QPainter *painter);

    Q_INVOKABLE void clearChart();

signals:
    void chartCleared();

private:
    QString url_;
};

#endif // PIECHART_H
