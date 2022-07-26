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
    void recieve_preview();

    void on_cl_slot0_clicked();

    void on_next_page_2_clicked();

public:
    remote_management(QWidget *parent = nullptr);
    ~remote_management();

private:

    Ui::MainWindow *ui;
    QUdpSocket* general_socket;

    struct protocol_msg_data {
        QString msg;
        // crypto
    };
    struct server_settings_data {
        QString msg;
        QString y_res;
        QString x_res;
        QString img_format;
        QString compression;
        QString preview_upd;
        QString xmit_upd;
    };

    //settings
    const QString y_res = 720;
    const QString x_res = 1280;
    const QString img_format = "JPG";
    const QString compression = "9";
    const QString preview_upd = "3"; // secs
    const QString xmit_upd = "0.05"; // secs
};
#endif // MAINWINDOW_H
