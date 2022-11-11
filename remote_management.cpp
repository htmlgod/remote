#include "remote_management.h"
#include "ui_remote_management.h"

quint16 CURRENT_CLIENT = 0;
QMap<quint16, QTcpSocket*> CLIENTS;
QMap<quint16, QPushButton*> CLIENT_TO_SLOT;
QHash<QPushButton*, quint16> SLOT_TO_CLIENT;
QQueue<QPushButton*> CLIENT_SLOTS;
QMap<quint16, QDataStream*> CLIENT_TO_DATASTREAM;
QMap<quint16, QHostAddress> CLIENT_TO_ADDRESS;

remote_management::remote_management(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mgm_socket = new mgm_server(this, ui->preview_scr);
    if (!mgm_socket->listen(QHostAddress::AnyIPv4, 1225)) {
        qDebug() << "ERROR OPEN SOCK";
        return;
    }
    ui->preview_scr->installEventFilter(this);
    control_socket = new QUdpSocket(this);
    CLIENT_SLOTS.append(ui->cl_slot0);
    CLIENT_SLOTS.append(ui->cl_slot1);
    CLIENT_SLOTS.append(ui->cl_slot2);
    CLIENT_SLOTS.append(ui->cl_slot3);
    CLIENT_SLOTS.append(ui->cl_slot4);
    CLIENT_SLOTS.append(ui->cl_slot5);
    for (auto slot : CLIENT_SLOTS) {
        connect(slot, SIGNAL(clicked()), this, SLOT(on_slotclicked()));
    }

}
bool remote_management::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->preview_scr and is_control) {
//        if (event->type() == QEvent::MouseMove) {
//            auto ev = static_cast<QMouseEvent *>(event);
//            QByteArray data;
//            QDataStream out(&data, QIODevice::WriteOnly);
//            control_data cd{"MOVE", ev->x(),ev->y()};
//            out << cd;
//            control_socket->writeDatagram(data, cl, CLIENT_CONTROL_PORT);
//            return true;
//        }
        if (event->type() == QEvent::MouseButtonPress) {
            auto ev = static_cast<QMouseEvent *>(event);
            //ui->screen->setText(ui->screen->text() + " pressed");
            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);
            QTransform tr;
            tr.scale(1680*1.0/ui->preview_scr->width(), 1050*1.0/ui->preview_scr->height());
            QPoint new_pos = tr.map(ev->pos());
            control_data cd{"CLICK", new_pos.x(),new_pos.y()};
            ev->pos();
            out << cd;
            control_socket->writeDatagram(data, cl, CLIENT_CONTROL_PORT);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}
remote_management::mgm_server::mgm_server(QObject *parent, QLabel* preview_scr) : QTcpServer(parent), preview_screen(preview_scr){}

void remote_management::mgm_server::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    QByteArray data;

    CLIENT_TO_DATASTREAM[cl_port]->startTransaction();
    *CLIENT_TO_DATASTREAM[cl_port] >> data;
    if(!CLIENT_TO_DATASTREAM[cl_port]->commitTransaction()) return;
    qDebug() << "incomming preview from" << cl_port;

    QPixmap pxm;
    QByteArray uncompressed = qUncompress(data);
    pxm.loadFromData(uncompressed, "JPG");

    if (CURRENT_CLIENT == cl_port) {
        preview_screen->setPixmap(pxm);
    }
    // fix later
    auto slot = CLIENT_TO_SLOT.value(cl_port);
    const int border_offset = 4;
    QIcon qi(pxm.scaled(slot->maximumSize().width() - border_offset, slot->maximumSize().height() - border_offset));
    slot->setIcon(qi);
    slot->setText("");
    slot->setIconSize(QSize(slot->maximumSize().width() - border_offset, slot->maximumSize().height() - border_offset));
    slot->update();
}

void remote_management::mgm_server::incomingConnection(qintptr socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    auto cl_port = client->peerPort();
    CLIENTS.insert(cl_port, client);

    qDebug() << "New client from:" << cl_port;
    CURRENT_CLIENT = cl_port;
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));

    auto slot = CLIENT_SLOTS.head();
    slot->setEnabled(true);
    CLIENT_TO_ADDRESS.insert(cl_port, client->peerAddress());
    CLIENT_SLOTS.pop_front();
    CLIENT_TO_SLOT.insert(cl_port, slot);
    SLOT_TO_CLIENT.insert(slot, cl_port);
    CLIENT_TO_DATASTREAM.insert(cl_port, new QDataStream);
    CLIENT_TO_DATASTREAM[cl_port]->setDevice(client);
    CLIENT_TO_DATASTREAM[cl_port]->setVersion(QDataStream::Qt_5_0);

    server_settings_data server_settings {
        y_res,x_res,img_format,compression, preview_upd, xmit_upd
    };
    QByteArray settings_data;
    QDataStream out(&settings_data, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_0);
    out << server_settings;
    client->write(settings_data);
    qDebug() << "Settings sended to " << cl_port;
}

void remote_management::mgm_server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    if (CURRENT_CLIENT == cl_port) {
        preview_screen->setPixmap(QPixmap());
        preview_screen->setText("ОЖИДАНИЕ КЛИЕНТА...");
        preview_screen->update();
    }
    CLIENTS.remove(cl_port);
    auto slot = CLIENT_TO_SLOT.take(cl_port);
    slot->setEnabled(false);
    slot->setIcon(QIcon());
    slot->setText("ОЖИДАНИЕ КЛИЕНТА...");
    slot->update();
    CLIENT_TO_SLOT.remove(cl_port);
    SLOT_TO_CLIENT.remove(slot);
    CLIENT_TO_DATASTREAM.remove(cl_port);
    CLIENT_SLOTS.push_front(slot);
    CLIENT_TO_ADDRESS.remove(cl_port);
    qDebug() << "Client " << cl_port << " disconnected";
}
remote_management::~remote_management()
{
    delete ui;
    delete mgm_socket;
}

void remote_management::start_control()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << QString("START");
    CLIENTS[CURRENT_CLIENT]->write(block);
    CLIENTS[CURRENT_CLIENT]->waitForBytesWritten();
}

void remote_management::stop_control()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << QString("STOP");
    CLIENTS[CURRENT_CLIENT]->write(block);
    CLIENTS[CURRENT_CLIENT]->waitForBytesWritten();

}

void remote_management::on_slotclicked()
{
    auto *slot = (QPushButton*)sender();
    ui->stackedWidget->setCurrentIndex(0);
    CURRENT_CLIENT = SLOT_TO_CLIENT[slot];
    CLIENTS[CURRENT_CLIENT]->waitForBytesWritten();
}

void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
void remote_management::on_next_page_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void remote_management::on_connect_button_clicked()
{
    cl = CLIENT_TO_ADDRESS[CURRENT_CLIENT];
    is_control = !is_control;
    if (is_control) {
        start_control();
        ui->connect_button->setText("Отключиться");
    }
    else {
        stop_control();
        ui->connect_button->setText("Подключиться");
    }


}
