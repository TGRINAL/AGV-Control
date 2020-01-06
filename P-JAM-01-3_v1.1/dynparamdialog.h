/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            dynparamdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef DYNPARAMDIALOG_H
#define DYNPARAMDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QStringList>
#include <QObject>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>

#include "dynparamtable.h"

namespace Ui {
class dynparamDialog;
}

/*
*Class Description
*/
class dynparamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit dynparamDialog(QStringList, QWidget *parent = 0);
    ~dynparamDialog();

private slots:
    void on_closeBtn_clicked();
    void on_writeBtn_clicked();

signals:
    void setDynparam(QString);

private:
    Ui::dynparamDialog *ui;
    QStringList oriValueList;
};

#endif // DYNPARAMDIALOG_H
