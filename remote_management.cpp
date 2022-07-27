#include "remote_management.h"
#include "ui_remote_management.h"

remote_management::remote_management(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    management_socket = new QUdpSocket(this);
    if (!management_socket->bind(QHostAddress::AnyIPv4, 1225)) {
        qDebug() << "ERROR OPEN SOCK";
        return;
    }
    connect(management_socket, &QUdpSocket::readyRead, this, &remote_management::recieve_management_data);
    ui->cl_slot0->setFocus();
}
void remote_management::process_preview(const QByteArray& data) {
    QPixmap pxm;
    QByteArray uncompressed = qUncompress(data);
    pxm.loadFromData(uncompressed, "JPG");

    ui->preview_scr->setPixmap(pxm);

    // fix later
    auto pxm_scaled = pxm.scaled(ui->cl_slot0->size().height() - 5, ui->cl_slot0->size().width() - 5);
    QIcon qi(pxm_scaled);
    qDebug() << "icon size" << qi.actualSize(ui->cl_slot0->size());
    ui->cl_slot0->setIcon(qi);
    ui->cl_slot0->setText("");
    ui->cl_slot0->setIconSize(pxm_scaled.rect().size());
    ui->cl_slot0->update();
}
void remote_management::process_client(const QNetworkDatagram& dg) {
    QByteArray data = dg.data();
    QDataStream ds(&data, QIODevice::ReadOnly);
    protocol_msg_data msg;
    ds >> msg;
    if (msg.msg == "SCR") {
        qDebug() << "Incoming preview from client " << dg.senderPort();
        process_preview(msg.data);
    }
    else if (msg.msg == "DSC") {
        qDebug() << "Pending disconnection from client " << dg.senderPort();
        process_client_disconnect(dg);
        qDebug() << "Client " << dg.senderPort() << " disconnected";
    }
    else if (msg.msg == "CON") {
        qDebug() << "Pending connection from client " << dg.senderPort();
        process_new_client(dg);
        qDebug() << "Client " << dg.senderPort() << " connected";
    }
    else {
        protocol_msg_data ans = {
            "ERR",
            0
        };
        QByteArray data;
        QDataStream answer(&data, QIODevice::WriteOnly);
        answer << ans;
        management_socket->writeDatagram(data, dg.senderAddress(), dg.senderPort());
        qDebug() << "Client " << dg.senderPort() << " failed to connect: ";
    }

}
void remote_management::process_new_client(const QNetworkDatagram& dg) {
    protocol_msg_data ans = {
        "OK",
        0
    };
    if (clients.contains(dg.senderPort())) {
        ans.msg = "ERR";
    }
    else {
        clients.insert(dg.senderPort());

    }
    QByteArray ans_data;
    QDataStream answer(&ans_data, QIODevice::WriteOnly);
    answer << ans;

    management_socket->writeDatagram(ans_data, dg.senderAddress(), dg.senderPort());
    qDebug() << "Connection request answer sended to " << dg.senderPort();
    server_settings_data server_settings {
        y_res,x_res,img_format,compression, preview_upd, xmit_upd
    };
    QByteArray settings_data;
    QDataStream settings(&settings_data, QIODevice::WriteOnly);
    settings << server_settings;
    management_socket->writeDatagram(settings_data, dg.senderAddress(), dg.senderPort());
    qDebug() << "Settings sended to " << dg.senderPort();
}
void remote_management::process_client_disconnect(const QNetworkDatagram& dg) {
    protocol_msg_data ans = {
        "OK",
        0
    };
    if (!clients.contains(dg.senderPort())) {
        ans.msg = "ERR";
    }
    else {
        clients.remove(dg.senderPort());
        qDebug() << "Client " << dg.senderPort() << " disconnected";
    }
    QByteArray data;
    QDataStream answer(&data, QIODevice::WriteOnly);
    answer << ans;
    management_socket->writeDatagram(data, dg.senderAddress(), dg.senderPort());
}
void remote_management::recieve_management_data() {
    // distinguish from which client came preview
    while (management_socket->hasPendingDatagrams()) {
        auto dg_size = management_socket->pendingDatagramSize();
        QNetworkDatagram dg = management_socket->receiveDatagram();

        std::stringstream ss;
        ss << "From " << dg.senderAddress().toString().toStdString();
        ss << ":" << dg.senderPort() << " recieved " << dg_size;
        qDebug() << QString::fromStdString(ss.str());

        process_client(dg);
    }
}

remote_management::~remote_management()
{
    delete ui;
}

void remote_management::on_cl_slot0_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
//    if (sender() == ui->cl_slot0) {

//    }
}

void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
