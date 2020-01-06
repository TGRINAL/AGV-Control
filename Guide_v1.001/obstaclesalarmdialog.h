/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            obstaclesalarmdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef OBSTACLESALARMDIALOG_H
#define OBSTACLESALARMDIALOG_H

#include <QDialog>

namespace Ui {
class ObstaclesAlarmDialog;
}

/*
* Class Description
*/
class ObstaclesAlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObstaclesAlarmDialog(QWidget *parent = nullptr);
    ~ObstaclesAlarmDialog();

private slots:
    void on_closeBtn_clicked();

private:
    Ui::ObstaclesAlarmDialog *ui;
};

#endif // OBSTACLESALARMDIALOG_H
