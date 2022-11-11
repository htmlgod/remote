#include "remote_control.h"
#include "ui_remote_control.h"

remote_control::remote_control(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::remote_control)
{
    ui->setupUi(this);
    ui->screen->installEventFilter(this);
}

bool remote_control::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->screen) {
        if (event->type() == QEvent::MouseMove) {
            auto ev = static_cast<QMouseEvent *>(event);
            ui->screen->setText(QString::number(ev->pos().x()) + ',' + QString::number(ev->pos().y()));
            return true;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            auto ev = static_cast<QMouseEvent *>(event);
            ui->screen->setText(ui->screen->text() + " pressed");
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}

remote_control::~remote_control()
{
    delete ui;
}
