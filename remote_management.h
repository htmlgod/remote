#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <QUdpSocket>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>

#include <QPixmap>
#include <QCursor>
#include <QWheelEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


const int CLIENT_CONTROL_PORT = 1235;
extern quint16 CURRENT_CLIENT;
extern QMap<quint16, QTcpSocket*> CLIENTS;
extern QMap<quint16, QPushButton*> CLIENT_TO_SLOT;
extern QMap<quint16, QDataStream*> CLIENT_TO_DATASTREAM;
extern QHash<QPushButton*, quint16> SLOT_TO_CLIENT;
extern QQueue<QPushButton*> CLIENT_SLOTS;
extern QMap<quint16, QHostAddress> CLIENT_TO_ADDRESS;

const QString y_res = "720";
const QString x_res = "1280";
const QString img_format = "JPG";
const QString compression = "9";
const QString preview_upd = "2"; // secs
const QString xmit_upd = "0.04"; // secs

void gen_key_for_client();
void encrypt(QByteArray data);
void decrypt(QByteArray data);

struct mouse_control_data {
    QString type; // move, click, wheel
    uint button = 0; // 1 -lmb, 3 - rmb, 2 - mmb, 0 - move
    int xpos;
    int ypos;
    int delta;
    friend QDataStream &operator<<(QDataStream& out, const mouse_control_data& cd){
        out << cd.type << cd.button << cd.xpos <<  cd.ypos << cd.delta;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, mouse_control_data& cd){
        in >> cd.type >> cd.button >> cd.xpos >>  cd.ypos >> cd.delta;
        return in;
    }
};
struct keyboard_control_data {
    QString type; // press, release
    int key = 0; // 1 -lmb, 3 - rmb, 2 - mmb, 0 - move
    QString text;
    friend QDataStream &operator<<(QDataStream& out, const keyboard_control_data& cd){
        out << cd.type << cd.key << cd.text;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, keyboard_control_data& cd){
        in >> cd.type >> cd.key >> cd.text;
        return in;
    }
};

struct control_data {
    QString type;
    mouse_control_data md;
    keyboard_control_data kd;
    friend QDataStream &operator<<(QDataStream& out, const control_data& cd){
        out << cd.type << cd.md << cd.kd;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, control_data& cd){
        in >> cd.type >> cd.md >> cd.kd;
        return in;
    }
};


struct server_settings_data {

    QString y_res;
    QString x_res;
    QString img_format;
    QString compression;
    QString preview_upd;
    QString xmit_upd;
    friend QDataStream &operator<<(QDataStream& out, const server_settings_data& server_settings){
        out << server_settings.y_res <<  server_settings.x_res <<  server_settings.img_format <<  server_settings.compression <<  server_settings.preview_upd <<  server_settings.xmit_upd;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, server_settings_data& server_settings){
        in >> server_settings.y_res >>  server_settings.x_res >>  server_settings.img_format >>  server_settings.compression >>  server_settings.preview_upd >>  server_settings.xmit_upd;
        return in;
    }
};

class remote_management : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_slotclicked();
    void on_next_page_2_clicked();
    void on_next_page_clicked();
    void on_connect_button_clicked();

public:
    remote_management(QWidget *parent = nullptr);

    bool eventFilter(QObject* target, QEvent* event) override;
    void start_control();
    void stop_control();

    ~remote_management();
private:
    QPoint translate_coordinates(const QPoint& mouse_pos);
    void send_controls(const control_data& data);
    void send_msg_to_cur_client(const QString& msg);


    Ui::MainWindow *ui;

    class mgm_server;
    mgm_server* mgm_socket;

    // add client info (screen resolution, OS?)
    int cl_desktop_width = 1680;
    int cl_desktop_height = 1050;
    QUdpSocket* control_socket;
    QHostAddress cl;
    bool is_control = false;
};


class remote_management::mgm_server : public QTcpServer
{
    Q_OBJECT

public:
    mgm_server(QObject *parent=0, QLabel* preview_screen=0);

private slots:
    void readyRead();
    void disconnected();

protected:
    void incomingConnection(qintptr socketfd);
private:
    QLabel* preview_screen;
    QDataStream in;
};

#endif // MAINWINDOW_H
