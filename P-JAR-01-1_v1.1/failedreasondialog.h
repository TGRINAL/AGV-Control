/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            failedreasondialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef FAILEDREASONDIALOG_H
#define FAILEDREASONDIALOG_H

#include <QDialog>

enum FAILED_REASON {ChannelObstacle = 11, FieldChange, GetLost, ArmError, MealError, NotGoing, Other, EngineeringTest};

namespace Ui {
class FailedReasonDialog;
}

/*
* Class Description
*/
class FailedReasonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FailedReasonDialog(QWidget *parent = nullptr);
    ~FailedReasonDialog();

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

signals:
    void parseFailedReason(FAILED_REASON);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::FailedReasonDialog *ui;
};

#endif // FAILEDREASONDIALOG_H
