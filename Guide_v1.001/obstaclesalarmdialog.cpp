/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            obstaclesalarmdialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "obstaclesalarmdialog.h"
#include "ui_obstaclesalarmdialog.h"

ObstaclesAlarmDialog::ObstaclesAlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObstaclesAlarmDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
}

ObstaclesAlarmDialog::~ObstaclesAlarmDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void ObstaclesAlarmDialog::on_closeBtn_clicked()
{
    this->close();
}
