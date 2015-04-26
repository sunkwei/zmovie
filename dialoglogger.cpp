#include "dialoglogger.h"
#include "ui_dialoglogger.h"
#include <stdio.h>
#include <stdarg.h>

DialogLogger::DialogLogger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLogger)
{
    ui->setupUi(this);
}

DialogLogger::~DialogLogger()
{
    delete ui;
}

void DialogLogger::log(int level, const char *fmt, ...)
{
    va_list args;
    char buf[4096];

    (void)level;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    QTextEdit *txt = this->findChild<QTextEdit*>("textEdit");
    txt->textCursor().insertText(buf);
}
