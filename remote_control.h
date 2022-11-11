#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QUdpSocket>
#include <QNetworkDatagram>

const int CLIENT_CONTROL_PORT = 1235;
const int XMIT_PORT = 1245;

namespace Ui {
class remote_control;
}

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

class remote_control : public QMainWindow
{
    Q_OBJECT

public:
    explicit remote_control(QHostAddress cl, QWidget *parent = nullptr);
    bool eventFilter(QObject* target, QEvent* event) override;
    ~remote_control();
private slots:
    void recieve_frame();
private:
    Ui::remote_control *ui;
    QUdpSocket* control_socket;
    QHostAddress cl;
};

#endif // REMOTE_CONTROL_H
