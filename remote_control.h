#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include <QWidget>

namespace Ui {
class remote_control;
}

class remote_control : public QWidget
{
    Q_OBJECT

public:
    explicit remote_control(QWidget *parent = 0);
    ~remote_control();

private:
    Ui::remote_control *ui;
};

#endif // REMOTE_CONTROL_H
