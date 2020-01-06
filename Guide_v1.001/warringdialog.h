/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            warringdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef WARRINGDIALOG_H
#define WARRINGDIALOG_H

#include <QDialog>

namespace Ui {
class WarringDialog;
}

/*
*Class Description
*/
class WarringDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WarringDialog(QWidget *parent = nullptr);
    ~WarringDialog();
    void setMessage(QString);
    void setCancelBtn(bool);

private slots:
    void on_cancelBtn_clicked();
    void on_okBtn_clicked();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::WarringDialog *ui;
};

#endif // WARRINGDIALOG_H
