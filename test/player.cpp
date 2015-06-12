#include "player.h"
#include <QQuickWindow>
#include "zkrender.h"

Player::Player()
{
}

void Player::setUrl(const QString &url)
{
    url_ = url;
}

QString Player::url() const
{
    return url_;
}

void Player::play()
{
    render_ = zkr_open_with_wnd(url_.toStdString().c_str(), (void*)window()->winId());
    zkr_set_window_rect(render_, x(), y(), width(), height());
}

void Player::stop()
{

}
