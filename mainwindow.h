#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "dialoglogger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_open_inputs_triggered();

private:
    Ui::MainWindow *ui;
    DialogLogger *log_dlg_;

private:
    void open_sources(const std::vector<std::string> &fnames, std::vector<int> &result);
    int open_source(const char *fname);
};

#endif // MAINWINDOW_H
