#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include <QMainWindow>
#include <QMouseEvent>

namespace Ui {
class remote_control;
}

class remote_control : public QMainWindow
{
    Q_OBJECT

public:
    explicit remote_control(QWidget *parent = nullptr);
    bool eventFilter(QObject* target, QEvent* event) override;
    ~remote_control();

private:
    Ui::remote_control *ui;
};

#endif // REMOTE_CONTROL_H
