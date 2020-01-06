/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            joystickdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef JOYSTICKDIALOG_H
#define JOYSTICKDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

#include "warringdialog.h"

enum VEL_TYPE {Forward, Backward, Right, Left, Stop};
enum TELEOP_TYPE {JOG, LINEAR};

namespace Ui {
class JoystickDialog;
}

/*
* Class Descriptio
*/
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

    void on_safeBtn_clicked();

    void on_ledLeftBtn_clicked();

    void on_ledRightBtn_clicked();

    void on_audioLeftBtn_clicked();

    void on_audioRightBtn_clicked();

    void on_ledPlayBtn_clicked();

    void on_audioPlayBtn_clicked();

    void on_showBtn_clicked();

    void on_audioLoopBtn_clicked();

public slots:
    void getVel(float, float);

private:
    Ui::JoystickDialog *ui;
    TELEOP_TYPE teleopMode;
    QTimer *backgroundTimer;
    bool flickerFlag;
    float linearValue = 0.0, angularValue = 0.0;

protected:
    void changeEvent(QEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void updateVel(VEL_TYPE);
    void setTeleopMode(TELEOP_TYPE);
    void setKeyPriorityFlag(bool);
    void abortMovebase();
    void showAction(int, int);
};

#endif // JOYSTICKDIALOG_H
