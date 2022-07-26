#include "remote_control.h"
#include "ui_remote_control.h"

remote_control::remote_control(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::remote_control)
{
    ui->setupUi(this);
}

remote_control::~remote_control()
{
    delete ui;
}
