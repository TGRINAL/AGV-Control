/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            editdbdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef EDITDBDIALOG_H
#define EDITDBDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>

enum DB_TYPE {DESK, CARD};

namespace Ui {
class EditDbDialog;
}

/*
* Class Description
*/
class EditDbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDbDialog(bool flag, DB_TYPE type, QWidget *parent = 0);
    EditDbDialog(bool flag, DB_TYPE type, QString id, QWidget *parent = 0);
    EditDbDialog(bool flag, DB_TYPE type, QString num, QString id, QWidget *parent = 0);
    ~EditDbDialog();

signals:
    void addDbFinish(QString, QString);
    void editDbFinish(QString, QString);

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::EditDbDialog *ui;
    QString number;
    QString identifier;
    bool isAddDb;
    DB_TYPE m_type;
};

#endif // EDITDBDIALOG_H
