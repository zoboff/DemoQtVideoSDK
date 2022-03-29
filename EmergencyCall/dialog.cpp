#include "dialog.h"
#include "ui_dialog.h"
#include <QDesktopWidget>
#include <QMovie>

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
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    m_sdk.open_session("127.0.0.1", "123");
    m_sdk.call("echotest@trueconf.com");
}
