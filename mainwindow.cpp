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
    connect(m_sdk, SIGNAL(change_state(State)), this, SLOT(on_change_state(State)));

    ui->connectButton->setEnabled(false);
    ui->callButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete m_sdk;
    delete ui;
}

void MainWindow::updateInterfaceForState(const State &state)
{
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/images/rc/article_18dp.png"), "Change state: " + QString::number(state));
    ui->listWidget->addItem(item);

    switch(state)
    {
        // --- No connection to the server and TrueConf Room does nothing
        case State::none:
            ui->stateLabel->setText("none");
            ui->stateLabel->setStyleSheet("QLabel { background-color : gray; color : black; }");
            break;
        // --- Tries to connect to the server
        case State::connect: ;
            ui->stateLabel->setText("connect");
            ui->stateLabel->setStyleSheet("QLabel { background-color : silver; color : black; }");
            break;
        // --- you need to login
        case State::login:
            ui->stateLabel->setText("login");
            ui->stateLabel->setStyleSheet("QLabel { background-color : blue; color : white; }");
            break;
        // --- Connected to the server and logged in
        case State::normal:
            ui->stateLabel->setText("normal");
            ui->stateLabel->setStyleSheet("QLabel { background-color : green; color : white; }");
            break;
        // -- Pending: either it calls somebody or somebody calls it
        case State::wait:
            ui->stateLabel->setText("wait");
            ui->stateLabel->setStyleSheet("QLabel { background-color : yellow; color : black; }");
            break;
        // --- In the conference
        case State::conference:
            ui->stateLabel->setText("conference");
            ui->stateLabel->setStyleSheet("QLabel { background-color : yellow; color : red; }");
            break;
        // --- session close
        case State::close:
            ui->stateLabel->setText("close");
            ui->stateLabel->setStyleSheet("QLabel { background-color : yellow; color : red; }");
            break;
        // ---
        default:
            ui->stateLabel->setText("Unknown!");
            ui->stateLabel->setStyleSheet("QLabel { background-color : gray; color : black; }");
    }

    ui->callButton->setEnabled(state == State::normal);
}

void MainWindow::on_start()
{
    ui->connectButton->setEnabled(true);

    qDebug() << "Started" << Qt::endl;
}

void MainWindow::on_stop()
{
    ui->connectButton->setEnabled(false);

    qDebug() << "Stopped" << Qt::endl;
}

void MainWindow::on_error(QString text)
{
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/images/rc/pan_tool_18dp.png"), "Error: " + text);
    ui->listWidget->addItem(item);

    qDebug() << "Error: " << text << Qt::endl;
}

void MainWindow::on_change_state(const State& state)
{
    /* Update interface */
    updateInterfaceForState(state);

    qDebug() << "Change state: " << QString::number(state) << Qt::endl;
}

void MainWindow::on_openButton_clicked()
{
    m_sdk->open(ui->edIP->text(), ui->edPIN->text());
}


void MainWindow::on_callButton_clicked()
{
    m_sdk->call(ui->edPeerId->text());
}


void MainWindow::on_connectButton_clicked()
{
    m_sdk->connectToServer(ui->edServerIP->text());
}


void MainWindow::on_loginButton_clicked()
{
    m_sdk->login(ui->edLogin->text(), ui->edPassword->text());
}

