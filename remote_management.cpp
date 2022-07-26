#include "remote_management.h"
#include "ui_remote_management.h"

remote_management::remote_management(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    general_socket = new QUdpSocket(this);
    general_socket->bind(QHostAddress::AnyIPv4, 1225);
    connect(general_socket, &QUdpSocket::readyRead, this, &remote_management::recieve_preview);
    ui->cl_slot0->setFocus();
}

void remote_management::recieve_preview() {
    // distinguish from which client came preview
    while (general_socket->hasPendingDatagrams()) {
        auto dg_size = general_socket->pendingDatagramSize();
        QNetworkDatagram dg = general_socket->receiveDatagram();

        std::stringstream ss;
        ss << "From " << dg.senderAddress().toString().toStdString();
        ss << ":" << dg.senderPort() << " recieved " << dg_size;
        qDebug() << QString::fromStdString(ss.str());

        QPixmap pxm;
        QByteArray uncompressed = qUncompress(dg.data());
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
}

remote_management::~remote_management()
{
    delete ui;
}

void remote_management::on_cl_slot0_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if (sender() == ui->cl_slot0) {

    }
}

void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
