#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tabviewercontainer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->Req_1panel, &QAction::triggered,
                     ui->centralwidget, &TabViewerContainer::make_singleviewertab);

    QObject::connect(ui->Req_2panel, &QAction::triggered,
                     ui->centralwidget, &TabViewerContainer::make_doubleviewertab);

    QObject::connect(ui->Req_3panel, &QAction::triggered,
                     ui->centralwidget, &TabViewerContainer::make_tripleviewertab);

}

MainWindow::~MainWindow()
{
    delete ui;
}



