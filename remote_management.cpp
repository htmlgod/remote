#include "remote_management.h"
#include "ui_remote_management.h"

qint16 cur_client;
QSet<QTcpSocket*> CLIENTS;
QMap<qint16, QPushButton*> CLIENT_TO_SLOT;
QMap<QPushButton*, qint16> SLOT_TO_CLIENT;
QQueue<QPushButton*> CLIENT_SLOTS;


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
}
// WIP UDP TO TCP XLATE
remote_management::mgm_server::mgm_server(QObject *parent, QLabel* preview_scr) : QTcpServer(parent), preview_screen(preview_scr){}

void remote_management::mgm_server::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    auto data = client->readAll();
    QPixmap pxm;
    QByteArray uncompressed = qUncompress(data);
    pxm.loadFromData(uncompressed, "JPG");
//if (cur_client == cl_port)
    preview_screen->setPixmap(pxm);

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

void remote_management::mgm_server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();

    CLIENTS.remove(client);
    auto tmp = CLIENT_TO_SLOT.take(cl_port);
    tmp->setIcon(QIcon());
    tmp->setText("ОЖИДАНИЕ КЛИЕНТА...");
    tmp->update();
    CLIENT_TO_SLOT.remove(cl_port);
    SLOT_TO_CLIENT.remove(tmp);
    CLIENT_SLOTS.append(tmp);
    qDebug() << "Client " << cl_port << " disconnected";
}

void remote_management::mgm_server::incomingConnection(qintptr socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    CLIENTS.insert(client);

    qDebug() << "New client from:" << client->peerPort();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    auto cl_port = client->peerPort();
     cur_client = cl_port;
    auto tmp = CLIENT_SLOTS.head();
    CLIENT_SLOTS.pop_front();
    CLIENT_TO_SLOT.insert(cl_port, tmp);
    SLOT_TO_CLIENT.insert(tmp, cl_port);

    server_settings_data server_settings {
        y_res,x_res,img_format,compression, preview_upd, xmit_upd
    };
    QByteArray settings_data;
    QDataStream settings(&settings_data, QIODevice::ReadWrite);
    settings.setVersion(QDataStream::Qt_5_10);
    settings << server_settings;
    client->write(settings_data);
    qDebug() << "Settings sended to " << cl_port;
}

remote_management::~remote_management()
{
    delete ui;
    delete mgm_socket;
}

void remote_management::on_cl_slot0_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    cur_client = SLOT_TO_CLIENT.value(ui->cl_slot0);
}

void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}

void remote_management::on_cl_slot1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    cur_client = SLOT_TO_CLIENT.value(ui->cl_slot1);
}

void remote_management::on_cl_slot2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    cur_client = SLOT_TO_CLIENT.value(ui->cl_slot2);
}


