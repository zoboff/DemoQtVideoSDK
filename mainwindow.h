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

    void updateInterfaceForState(const State& state);

private slots:
    void on_openButton_clicked();
    void on_callButton_clicked();
    void on_connectButton_clicked();
    void on_loginButton_clicked();

    void on_logoutButton_clicked();

private:
    Ui::MainWindow *ui;
    VideoSDK *m_sdk;

public slots:
    void on_start();
    void on_stop();
    void on_error(QString text);
    void on_change_state(const State& state);

};
#endif // MAINWINDOW_H
