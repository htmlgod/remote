#include "remote_management.h"
#include "ui_remote_management.h"

quint16 CURRENT_CLIENT = 0;
QSet<QTcpSocket*> CLIENTS;
QMap<quint16, QPushButton*> CLIENT_TO_SLOT;
QMap<QPushButton*, quint16> SLOT_TO_CLIENT;
QQueue<QPushButton*> CLIENT_SLOTS;
QMap<quint16, QDataStream*> CLIENT_TO_DATASTREAM;

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
    ui->cl_slot0->setFocus();
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
remote_management::mgm_server::mgm_server(QObject *parent, QLabel* preview_scr) : QTcpServer(parent), preview_screen(preview_scr){}

void remote_management::mgm_server::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    QByteArray data;

    CLIENT_TO_DATASTREAM[cl_port]->startTransaction();
    *CLIENT_TO_DATASTREAM[cl_port] >> data;
    if(!CLIENT_TO_DATASTREAM[cl_port]->commitTransaction()) return;
    qDebug() << "incomming preview from " << cl_port;

    QPixmap pxm;
    QByteArray uncompressed = qUncompress(data);
    pxm.loadFromData(uncompressed, "JPG");

    if (CURRENT_CLIENT == cl_port) {
        preview_screen->setPixmap(pxm);
    }
    // fix later
    //auto pxm_scaled = pxm.scaled(ui->cl_slot0->size().height() - 5, ui->cl_slot0->size().width() - 5);
    //QIcon qi(pxm_scaled);
    QIcon qi(pxm);
    //qDebug() << "icon size" << qi.actualSize(ui->cl_slot0->size());
    auto tmp = CLIENT_TO_SLOT.value(cl_port);
    tmp->setIcon(qi);
    tmp->setText("");
    //tmp->setIconSize(pxm_scaled.rect().size());
    tmp->setIconSize(pxm.rect().size());
    tmp->update();
}

void remote_management::mgm_server::incomingConnection(qintptr socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    CLIENTS.insert(client);
    auto cl_port = client->peerPort();
    qDebug() << "New client from:" << cl_port;
    CURRENT_CLIENT = cl_port;
    qDebug() << CURRENT_CLIENT;
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));

    auto slot = CLIENT_SLOTS.head();
    slot->setEnabled(true);
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
    QDataStream settings(&settings_data, QIODevice::ReadWrite);
    settings.setVersion(QDataStream::Qt_5_0);
    settings << server_settings;
    client->write(settings_data);
    qDebug() << "Settings sended to " << cl_port;
}

void remote_management::mgm_server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    CLIENTS.remove(client);
    auto slot = CLIENT_TO_SLOT.take(cl_port);
    slot->setEnabled(false);
    slot->setIcon(QIcon());
    slot->setText("ОЖИДАНИЕ КЛИЕНТА...");
    slot->update();
    CLIENT_TO_SLOT.remove(cl_port);
    SLOT_TO_CLIENT.remove(slot);
    CLIENT_TO_DATASTREAM.remove(cl_port);
    CLIENT_SLOTS.push_front(slot);
    qDebug() << "Client " << cl_port << " disconnected";
}
remote_management::~remote_management()
{
    delete ui;
    delete mgm_socket;
}

void remote_management::on_slotclicked()
{
    auto *slot = (QPushButton*)sender();
    ui->stackedWidget->setCurrentIndex(0);
    CURRENT_CLIENT = SLOT_TO_CLIENT[slot];
}

void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
void remote_management::on_next_page_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
