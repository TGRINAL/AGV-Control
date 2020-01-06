/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            performancedialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef PERFORMANCEDIALOG_H
#define PERFORMANCEDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSqlRelationalTableModel>

#include "joystickdialog.h"

enum PERFORMANCE_TYPE {ARM_TYPE = 31, AUDIO_TYPE, LED_TYPE, DELIVERY_TYPE, ABORT_TYPE, GOORIGIN_TYPE, RESETORIGIN_TYPE};

namespace Ui {
class PerformanceDialog;
}

/*
* Class Description
*/
class PerformanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PerformanceDialog(QSqlRelationalTableModel *, QWidget *parent = nullptr);
    ~PerformanceDialog();

private slots:
    void on_forwardBtn_pressed();
    void on_backwardBtn_pressed();
    void on_leftBtn_pressed();
    void on_stopBtn_clicked();
    void on_rightBtn_pressed();
    void on_safeStopBtn_clicked();
    void on_armBtn_clicked();
    void on_audioBtn_clicked();
    void on_ledBtn_clicked();
    void on_abortBtn_clicked();
    void on_goOriginBtn_clicked();
    void on_resetOriginBtn_clicked();
    void on_closeBtn_clicked();
    void on_deliveryBtn_clicked();

public slots:
    void updateDeliveryStatus(bool);

private:
    Ui::PerformanceDialog *ui;
    QTimer *armTimer;
    QTimer *audioTimer;
    QTimer *ledTimer;
    int isDeliveryFlag;

    void handleArmTimeout();
    void handleAudioTimeout();
    void handleLedTimeout();

protected:
    void changeEvent(QEvent *event);

signals:
    void updateVel(VEL_TYPE);
    void setKeyPriorityFlag(bool);
    void requestPerformance(PERFORMANCE_TYPE, int);
};

#endif // PERFORMANCEDIALOG_H
