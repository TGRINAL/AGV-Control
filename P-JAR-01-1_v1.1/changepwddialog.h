/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            changepwddialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef CHANGEPWDDIALOG_H
#define CHANGEPWDDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class ChangePwdDialog;
}

/*
* Class Description
*/
class ChangePwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwdDialog(QWidget *parent = nullptr);
    ~ChangePwdDialog();
    static QString getOnePassword(QWidget *parent = nullptr, bool *ok = nullptr);

private slots:
    void on_cancelBtn_clicked();
    void on_confirmBtn_clicked();

    void on_showPwCheckBox_toggled(bool checked);

signals:
    void setNewPassword(QString);

private:
    Ui::ChangePwdDialog *ui;
    void setOnePasswordMode();
    bool isOnePasswordMode;
};

#endif // CHANGEPWDDIALOG_H
