#include "dialog.h"
#include "ui_dialog.h"
#include <QDesktopWidget>
#include <QMovie>
#include <QDebug>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    /* Set transparent */
    setStyleSheet("background: transparent");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    /* Set UI position */
    QDesktopWidget desktop_widget;
    QRect rcScreen = desktop_widget.availableGeometry(desktop_widget.primaryScreen());

    this->move(rcScreen.width() - this->width(), rcScreen.height() - this->height());

    /* Animated button */
    auto movie = new QMovie(this);

    movie->setFileName(":/rc/ambulance-hospital.gif");
    connect(movie, &QMovie::frameChanged, [=]{
        ui->pushButton->setIcon(movie->currentPixmap());
    });
    movie->start();

    /*  */
    connect(&m_sdk, &VideoSDK::change_state, [=](State state){
        m_state = state;
        if(m_state == State::conference || m_state == State::wait)
            m_sdk.changeWindowState(WindowState::maximized, true);
        else
            m_sdk.changeWindowState(WindowState::minimized, false);
    });

    /* When Room or VideoSDK have been started on local machine */
    QString room_ip = "127.0.0.1";

    m_sdk.open_session(room_ip);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    m_sdk.call("echotest@trueconf.com");
}
