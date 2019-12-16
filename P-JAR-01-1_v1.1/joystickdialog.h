#ifndef JOYSTICKDIALOG_H
#define JOYSTICKDIALOG_H

#include <QDialog>
#include <QTimer>

enum VEL_TYPE {Forward, Backward, Right, Left, Stop};
enum TELEOP_TYPE {JOG, LINEAR};

namespace Ui {
class JoystickDialog;
}

class JoystickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoystickDialog(bool flicker, QWidget *parent = nullptr);
    ~JoystickDialog();
    void setFlicker(bool);

private slots:
    void handleBackgroundTimeout();

    void on_closeBtn_clicked();

    void on_forwardBtn_pressed();

    void on_backwardBtn_pressed();

    void on_leftBtn_pressed();

    void on_rightBtn_pressed();

    void on_forwardBtn_released();

    void on_backwardBtn_released();

    void on_leftBtn_released();

    void on_rightBtn_released();

    void on_stopBtn_clicked();

    void on_jogRb_toggled(bool checked);

    void on_abortBtn_clicked();

    void on_goOriginBtn_clicked();

    void on_resetOriginBtn_clicked();

    void on_safeBtn_clicked();

private:
    Ui::JoystickDialog *ui;
    TELEOP_TYPE teleopMode;
    QTimer *backgroundTimer;
    bool flickerFlag;

protected:
    void changeEvent(QEvent *event);

signals:
    void updateVel(VEL_TYPE);
    void setTeleopMode(TELEOP_TYPE);
    void setKeyPriorityFlag(bool);
    void abortMovebase();
    void goOrigin();
    void resetOrigin();
};

#endif // JOYSTICKDIALOG_H
