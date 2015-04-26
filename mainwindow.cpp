#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"

extern DialogLogger *_logger;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    log_dlg_ = new DialogLogger(this);
    log_dlg_->show();

    _logger = log_dlg_;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_inputs_triggered()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    if (dlg.exec() == 1) {
        std::vector<std::string> fnames;

        QStringList files = dlg.selectedFiles();
        for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it) {
            QString fname = *it;
            log_dlg_->log(9, "%s\n", fname.toUtf8().data());
            fnames.push_back(fname.toUtf8().data());
        }

        std::vector<int> results;
        open_sources(fnames, results);
    }
}

int MainWindow::open_source(const char *fname)
{
    return -1;
}

void MainWindow::open_sources(const std::vector<std::string> &fnames, std::vector<int> &result)
{

}
