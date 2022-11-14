#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include <QtNetwork>
#include <QShortcut>
#include <QPixmap>
#include <QCursor>
#include <QWheelEvent>

#include <common.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct client_info {
    int screenw;
    int screenh;
};


extern QMap<quint16, client_info> CLIENT_TO_INFO;
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



class remote_management : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_slotclicked();
    void on_next_page_2_clicked();
    void on_next_page_clicked();
    void on_connect_button_clicked();
    void toggle_fullscreen();

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


    void start_fullscreen();
    QShortcut* fullscreen_Ctrl_F = nullptr;
    void stop_fullscreen();

    Ui::MainWindow *ui;

    class mgm_server;
    mgm_server* mgm_socket;

    // add client info (screen resolution, OS?)
    int cl_desktop_width = 1680;
    int cl_desktop_height = 1050;
    QUdpSocket* control_socket;
    QHostAddress cl;
    bool is_control = false;
    bool is_fullscreen = true;
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
