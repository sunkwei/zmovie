#include "ptz.h"
#include <QtScript/QScriptEngine>

Ptz::Ptz()
{
    ptz_ = 0;
}

bool Ptz::left(int speed)
{
    return (ptz_ && ptz_left(ptz_, speed) >= 0);
}

bool Ptz::right(int speed)
{
    return (ptz_ && ptz_right(ptz_, speed) >= 0);
}

bool Ptz::up(int speed)
{
    return (ptz_ && ptz_up(ptz_, speed) >= 0);
}

bool Ptz::down(int speed)
{
    return (ptz_ && ptz_down(ptz_, speed) >= 0);
}

bool Ptz::stop()
{
    return (ptz_ && ptz_stop(ptz_) >= 0);
}

bool Ptz::preset_call(int id)
{
    return (ptz_ && ptz_preset_call(ptz_, id) >= 0);
}

bool Ptz::preset_clear(int id)
{
    return (ptz_ && ptz_preset_clr(ptz_, id) >= 0);
}

bool Ptz::preset_save(int id)
{
    return ptz_ && ptz_preset_save(ptz_, id) >= 0;
}

bool Ptz::set_pos(int x, int y, int speedx, int speedy)
{
    return ptz_ && ptz_set_absolute_position(ptz_, x, y, speedx) >= 0;
}

bool Ptz::set_zoom(int z)
{
    return ptz_ && ptz_zoom_set(ptz_, z) >= 0;
}

bool Ptz::zoom_stop()
{
    return ptz_ && ptz_zoom_stop(ptz_) >= 0;
}

bool Ptz::zoom_tele(int speed)
{
    return ptz_ && ptz_zoom_tele(ptz_, speed) >= 0;
}

bool Ptz::zoom_wide(int speed)
{
    return ptz_ && ptz_zoom_wide(ptz_, speed) >= 0;
}

QVariant Ptz::get_pos()
{
    QScriptEngine engine;
    QScriptValue rc = engine.newObject();

    int x, y;
    if (!ptz_ || ptz_get_current_position(ptz_, &x, &y) < 0) {
        rc.setProperty("err", -1);
    }
    else {
        rc.setProperty("err", 0);
        rc.setProperty("x", x);
        rc.setProperty("y", y);
    }

    return rc.toVariant();
}

QVariant Ptz::get_zoom()
{
    QScriptEngine engine;
    QScriptValue rc = engine.newObject();

    int z;
    if (!ptz_ || (z = ptz_zoom_get(ptz_)) < 0) {
        rc.setProperty("err", -1);
    }
    else {
        rc.setProperty("err", 0);
        rc.setProperty("zoom", z);
    }
    return rc.toVariant();
}
