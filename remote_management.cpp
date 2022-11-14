#include "remote_management.h"
#include "ui_remote_management.h"

quint16 CURRENT_CLIENT = 0;
QMap<quint16, QTcpSocket*> CLIENTS;
QMap<quint16, QPushButton*> CLIENT_TO_SLOT;
QHash<QPushButton*, quint16> SLOT_TO_CLIENT;
QQueue<QPushButton*> CLIENT_SLOTS;
QMap<quint16, QDataStream*> CLIENT_TO_DATASTREAM;
QMap<quint16, QHostAddress> CLIENT_TO_ADDRESS;
QMap<quint16, client_info> CLIENT_TO_INFO;

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
    ui->connect_button->setEnabled(false);

}
QPoint remote_management::translate_coordinates(const QPoint& mouse_pos) {
    QTransform tr;
    auto cl_info = CLIENT_TO_INFO[CURRENT_CLIENT];
    tr.scale(cl_info.screenw*1.0/ui->preview_scr->width(),
             cl_info.screenh*1.0/ui->preview_scr->height());
    return tr.map(mouse_pos);
}
void remote_management::send_controls(const control_data& cd) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << cd;
    control_socket->writeDatagram(data, cl, CLIENT_CONTROL_PORT);
}
bool remote_management::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->preview_scr and is_control) {
        if (event->type() == QEvent::MouseMove) {
            auto ev = static_cast<QMouseEvent*>(event);
            auto new_pos = translate_coordinates(ev->pos());
            mouse_control_data md{"MOVE", 0, new_pos.x(),new_pos.y(),0};
            control_data cd{"MOUSE", md, {}};
            send_controls(cd);
            return true;
        }
        else if (event->type() == QEvent::MouseButtonPress) {
            auto ev = static_cast<QMouseEvent*>(event);
            uint button = ev->button();
            mouse_control_data md{"PRESS", button, 0,0,0};
            control_data cd{"MOUSE", md, {}};
            send_controls(cd);
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            auto ev = static_cast<QMouseEvent*>(event);
            uint button = ev->button();
            mouse_control_data md{"RELEASE", button, 0,0,0};
            control_data cd{"MOUSE", md, {}};
            send_controls(cd);
            return true;
        }
        else if (event->type() == QEvent::Wheel) {
            auto ev = static_cast<QWheelEvent*>(event);
            mouse_control_data md{"SCROLL", 0,0,0,ev->delta()};
            control_data cd{"MOUSE", md, {}};
            send_controls(cd);
            return true;
        }
        else if (event->type() == QEvent::KeyPress) {
            auto ev = static_cast<QKeyEvent*>(event);
            keyboard_control_data kd {"PRESS", ev->key(), ev->text()};
            control_data cd{"KEYBOARD", {}, kd};
            send_controls(cd);
            return true;
        }
        else if (event->type() == QEvent::KeyRelease) {
            auto ev = static_cast<QKeyEvent*>(event);
            keyboard_control_data kd {"RELEASE", ev->key(), ev->text()};
            control_data cd{"KEYBOARD", {}, kd};
            send_controls(cd);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}
remote_management::mgm_server::mgm_server(QObject *parent, QLabel* preview_scr)
    : QTcpServer(parent),
    preview_screen(preview_scr){}

void remote_management::mgm_server::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    auto cl_port = client->peerPort();
    QByteArray data;

    CLIENT_TO_DATASTREAM[cl_port]->startTransaction();
    *CLIENT_TO_DATASTREAM[cl_port] >> data;
    if(!CLIENT_TO_DATASTREAM[cl_port]->commitTransaction()) return;
    //qDebug() << "incomming preview from" << cl_port;

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
    if (!CURRENT_CLIENT) CURRENT_CLIENT = cl_port;


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
    client->waitForReadyRead();
    QDataStream in(client);
    int w, h;
    in >> w >> h;
    qDebug() << "Recieved client info:" << w << h;
    CLIENT_TO_INFO.insert(cl_port, {w,h});
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
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
    CLIENT_TO_INFO.remove(cl_port);
    CLIENT_TO_SLOT.remove(cl_port);
    SLOT_TO_CLIENT.remove(slot);
    CLIENT_TO_DATASTREAM.remove(cl_port);
    CLIENT_SLOTS.push_front(slot);
    CLIENT_TO_ADDRESS.remove(cl_port);
    CURRENT_CLIENT = 0;
    qDebug() << "Client " << cl_port << " disconnected";
}
remote_management::~remote_management()
{
    delete ui;
    delete mgm_socket;
}
void remote_management::send_msg_to_cur_client(const QString &msg) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << msg;
    CLIENTS[CURRENT_CLIENT]->write(block);
    CLIENTS[CURRENT_CLIENT]->waitForBytesWritten();
}


void remote_management::start_control()
{
    send_msg_to_cur_client("START");
}
void remote_management::stop_control()
{
    send_msg_to_cur_client("STOP");
}
void remote_management::on_slotclicked()
{
    auto *slot = (QPushButton*)sender();
    ui->stackedWidget->setCurrentIndex(0);
    CURRENT_CLIENT = SLOT_TO_CLIENT[slot];
    if (CURRENT_CLIENT) {
        ui->connect_button->setEnabled(true);
    }
    else {
        ui->connect_button->setEnabled(false);
    }
}
void remote_management::on_next_page_2_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
void remote_management::on_next_page_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if (CURRENT_CLIENT) {
        ui->connect_button->setEnabled(true);
    }
    else {
        ui->connect_button->setEnabled(false);
    }
}
void remote_management::on_connect_button_clicked()
{
    cl = CLIENT_TO_ADDRESS[CURRENT_CLIENT];
    is_control = !is_control;
    if (is_control) {
        start_control();
        ui->connect_button->setText("Отключиться");
        QMessageBox::information(ui->preview_scr, "Удаленное управление", "Вы вошли в режим удаленного управления.\n"
                                                               "Чтобы выйти из полноэкранного режима, нажмите Ctrl+F");
        start_fullscreen();
    }
    else {
        stop_control();
        ui->connect_button->setText("Подключиться");
    }
}
void remote_management::start_fullscreen()
{
    ui->preview_scr->setWindowFlag(Qt::Window);
    ui->preview_scr->showFullScreen();
    if (fullscreen_Ctrl_F != nullptr) delete fullscreen_Ctrl_F;
    fullscreen_Ctrl_F = new QShortcut(QKeySequence(tr("Ctrl+F","Toggle Fullscreen")), ui->preview_scr);
    connect(fullscreen_Ctrl_F, SIGNAL(activated()), this, SLOT(toggle_fullscreen()));
    is_fullscreen = true;
}

void remote_management::stop_fullscreen()
{
    ui->preview_scr->setWindowFlag(Qt::Window, false);
    ui->preview_scr->show();
    if (fullscreen_Ctrl_F != nullptr) delete fullscreen_Ctrl_F;
    fullscreen_Ctrl_F = new QShortcut(QKeySequence(tr("Ctrl+F","Toggle Fullscreen")), this);
    connect(fullscreen_Ctrl_F, SIGNAL(activated()), this, SLOT(toggle_fullscreen()));
    is_fullscreen = false;
}
void remote_management::toggle_fullscreen()
{
    if (is_fullscreen) {
        stop_fullscreen();
    }
    else {
        start_fullscreen();
    }
}
