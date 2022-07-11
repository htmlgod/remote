#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sock = new QUdpSocket(this);
    sock->bind(QHostAddress::AnyIPv4, 1225);
    connect(sock, &QUdpSocket::readyRead, this, &MainWindow::recieve);
}

void MainWindow::recieve() {
    while (sock->hasPendingDatagrams()) {
        QNetworkDatagram dg = sock->receiveDatagram();
        std::stringstream ss;
        ss << "From " << dg.senderAddress().toString().toStdString();
        //ss << ":" << dg.senderPort() << " recieved screenshot";
        ss << ":" << dg.senderPort() << " recieved msg #" << dg.data().constData();
        ui->log->addItem(QString::fromStdString(ss.str()));
        //QPixmap pxm;
        //pxm.loadFromData(dg.data().constData(), "PNG");
        //ui->scr->setPixmap(pxm);
        //ui->scr->update();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

