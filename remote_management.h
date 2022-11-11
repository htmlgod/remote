#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QCursor>
#include <QMainWindow>
#include <QUdpSocket>
#include <QtNetwork>
#include <QtWidgets>

#include <QTcpServer>
#include <QTcpSocket>

#include <QPixmap>


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
struct control_data {
    QString type; // move, click
    int xpos;
    int ypos;
    friend QDataStream &operator<<(QDataStream& out, const control_data& cd){
        out << cd.type <<  cd.xpos <<  cd.ypos;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, control_data& cd){
        in >> cd.type >>  cd.xpos >>  cd.ypos;
        return in;
    }
};
struct protocol_msg_data {
    QString msg;
    QByteArray data;
    // crypto
    bool operator==(const protocol_msg_data& o){
        return o.msg == this->msg && o.data == this->data;

    }
    friend QDataStream &operator<<(QDataStream& out, const protocol_msg_data& rhs){
        out << rhs.data << rhs.msg;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, protocol_msg_data& rhs){
        in  >> rhs.data >> rhs.msg;
        return in;
    }
};
const QString y_res = "720";
const QString x_res = "1280";
const QString img_format = "JPG";
const QString compression = "9";
const QString preview_upd = "2"; // secs
const QString xmit_upd = "0.05"; // secs
struct server_settings_data {

    QString y_res;
    QString x_res;
    QString img_format;
    QString compression;
    QString preview_upd;
    QString xmit_upd;
    //bool operator==(const server_settings_data& o) const = default;
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
    ~remote_management();
    bool eventFilter(QObject* target, QEvent* event) override;
    void start_control();
    void stop_control();
private:
    class mgm_server;

    mgm_server* mgm_socket;
    QUdpSocket* control_socket;
    QHostAddress cl;
    Ui::MainWindow *ui;
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
