#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_sdk = new VideoSDK("127.0.0.1", this);
}

MainWindow::~MainWindow()
{
    delete m_sdk;
    delete ui;
}


void MainWindow::on_ConnectButton_clicked()
{

}
