#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCursor>
#include <QUdpSocket>
#include <QtNetwork>
#include <QtWidgets>
#include <sstream>
#include <zlib.h>
#include <QPixmap>
#include "remote_control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class remote_management : public QMainWindow
{
    Q_OBJECT

private slots:
    void recieve_management_data();

    void on_cl_slot0_clicked();

    void on_next_page_2_clicked();

public:
    remote_management(QWidget *parent = nullptr);
    ~remote_management();

    void process_preview(const QByteArray& data);
    void process_client(const QNetworkDatagram& dg);
    void process_new_client(const QNetworkDatagram& dg);
    void process_client_disconnect(const QNetworkDatagram& dg);
    void process_key_exchange(const QNetworkDatagram& dg);

private:
    Ui::MainWindow *ui;
    QUdpSocket* management_socket;

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

    struct server_settings_data {
        QString y_res;
        QString x_res;
        QString img_format;
        QString compression;
        QString preview_upd;
        QString xmit_upd;
        bool operator==(const server_settings_data& o) const = default;
        friend QDataStream &operator<<(QDataStream& out, const server_settings_data& server_settings){
            out << server_settings.y_res <<  server_settings.x_res <<  server_settings.img_format <<  server_settings.compression <<  server_settings.preview_upd <<  server_settings.xmit_upd;
            return out;
        }
        friend QDataStream &operator>>(QDataStream& in, server_settings_data& server_settings){
            in >> server_settings.y_res >>  server_settings.x_res >>  server_settings.img_format >>  server_settings.compression >>  server_settings.preview_upd >>  server_settings.xmit_upd;
            return in;
        }
    };
    QSet<int> clients;

    //settings
    const QString y_res = "480";
    const QString x_res = "640";
    const QString img_format = "JPG";
    const QString compression = "9";
    const QString preview_upd = "3"; // secs
    const QString xmit_upd = "0.05"; // secs
};
#endif // MAINWINDOW_H
