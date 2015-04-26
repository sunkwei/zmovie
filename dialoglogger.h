#ifndef DIALOGLOGGER_H
#define DIALOGLOGGER_H

#include <QDialog>

namespace Ui {
class DialogLogger;
}

class DialogLogger : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLogger(QWidget *parent = 0);
    ~DialogLogger();

    void log(int level, const char *fmt, ...);

private:
    Ui::DialogLogger *ui;
};

#endif // DIALOGLOGGER_H
