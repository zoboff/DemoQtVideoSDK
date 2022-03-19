#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videosdk.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ConnectButton_clicked();

private:
    Ui::MainWindow *ui;
    VideoSDK *m_sdk;

public slots:
    void on_start();
    void on_stop();
    void on_error(QString text);

};
#endif // MAINWINDOW_H
