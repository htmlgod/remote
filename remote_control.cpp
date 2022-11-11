#include "remote_control.h"
#include "ui_remote_control.h"

remote_control::remote_control(QHostAddress cl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::remote_control)
{
    ui->setupUi(this);
    ui->screen->installEventFilter(this);
    control_socket = new QUdpSocket(this);
    if(!control_socket->bind(QHostAddress::AnyIPv4, XMIT_PORT)) {
        qDebug() << "Error binding sock";
        exit(0);
    }
    connect(control_socket, SIGNAL(readyRead()), this, SLOT(recieve_frame()));
    this->cl = cl;
}

bool remote_control::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->screen) {
        if (event->type() == QEvent::MouseMove) {
            //auto ev = static_cast<QMouseEvent *>(event);
            //ui->screen->setText(QString::number(ev->pos().x()) + ',' + QString::number(ev->pos().y()));
            return true;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            //auto ev = static_cast<QMouseEvent *>(event);
            //ui->screen->setText(ui->screen->text() + " pressed");
            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);
            control_data cd{"CLICK", 0,0};
            out << cd;
            control_socket->writeDatagram(data, cl, CLIENT_CONTROL_PORT);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}

remote_control::~remote_control()
{
    control_socket->writeDatagram(QByteArray(QString("STOP").toUtf8()), cl, CLIENT_CONTROL_PORT);
    delete ui;
}

void remote_control::recieve_frame()
{
    if (control_socket->hasPendingDatagrams()) {
        auto dg = control_socket->receiveDatagram();
        auto data = dg.data();
        QPixmap pxm;
        QByteArray uncompressed = qUncompress(data);
        pxm.loadFromData(uncompressed, "JPG");
        ui->screen->setPixmap(pxm);
    }
}
