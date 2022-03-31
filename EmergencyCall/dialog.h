#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "../videosdk/videosdk.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    VideoSDK m_sdk {this};
    State m_state;
};
#endif // DIALOG_H
