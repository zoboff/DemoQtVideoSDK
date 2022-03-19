#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_sdk = new VideoSDK(this);

    connect(m_sdk, SIGNAL(opened()), this, SLOT(on_start()));
    connect(m_sdk, SIGNAL(closed()), this, SLOT(on_stop()));
    connect(m_sdk, SIGNAL(error(QString)), this, SLOT(on_error(QString)));
}

MainWindow::~MainWindow()
{
    delete m_sdk;
    delete ui;
}


void MainWindow::on_ConnectButton_clicked()
{
    m_sdk->open("127.0.0.1", "1234");
}

void MainWindow::on_start()
{
    qDebug() << "Started" << endl;
}

void MainWindow::on_stop()
{
    qDebug() << "Stopped" << endl;
}

void MainWindow::on_error(QString text)
{
    qDebug() << "Error: " << text << endl;
}
