#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QCursor>
#include <QUdpSocket>
#include <QtNetwork>
#include <QtWidgets>

#include <QTcpServer>
#include <QTcpSocket>

#include <QPixmap>

#include "remote_control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

extern quint16 CURRENT_CLIENT;
extern QSet<QTcpSocket*> CLIENTS;
extern QMap<quint16, QPushButton*> CLIENT_TO_SLOT;
extern QMap<quint16, QDataStream*> CLIENT_TO_DATASTREAM;
extern QHash<QPushButton*, quint16> SLOT_TO_CLIENT;
extern QQueue<QPushButton*> CLIENT_SLOTS;

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
const QString preview_upd = "6"; // secs
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

private:
    class mgm_server;

    mgm_server* mgm_socket;
    Ui::MainWindow *ui;
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
