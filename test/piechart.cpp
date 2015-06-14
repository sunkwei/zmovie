#include "piechart.h"
#include <QColor>
#include <QPen>
#include <QPainter>
#include <SDL2/SDL.h>
extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
}

PieChart::PieChart()
{
    av_register_all();
}

QString PieChart::url() const
{
    return url_;
}

void PieChart::seturl(const QString &url)
{
    url_ = url;
}

void PieChart::paint(QPainter *painter)
{
    QColor color(255, 0, 0);
    QPen pen(color, 2);

    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawPie(boundingRect().adjusted(1, 1, -1, -1), 90 * 16, 290 * 16);
}

void PieChart::clearChart()
{
    emit chartCleared();
}
