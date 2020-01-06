/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            recorddialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/
			
#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTime>
#include <QCalendarWidget>

namespace Ui {
class RecordDialog;
}

/*
* Class Description
*/
class RecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordDialog(QWidget *parent = nullptr);
    ~RecordDialog();

private slots:
    void on_filterBtn_clicked();
    void on_dateTimeEdit_1_dateTimeChanged(const QDateTime &dateTime);
    void on_dateTimeEdit_2_dateTimeChanged(const QDateTime &dateTime);

private:
    Ui::RecordDialog *ui;
    QFile serviceLogFile;
};

#endif // RECORDDIALOG_H
