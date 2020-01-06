/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            warringdialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "warringdialog.h"
#include "ui_warringdialog.h"

WarringDialog::WarringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarringDialog)
{
    ui->setupUi(this);
}

WarringDialog::~WarringDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	str
*Returns : 		void
*/
void WarringDialog::setMessage(QString str)
{
    ui->messageBox->setText(str);
}

/*
*Function Description :
*Parameters : 	flag
*Returns : 		void
*/
void WarringDialog::setCancelBtn(bool flag)
{
    ui->cancelBtn->setVisible(false);
    ui->cancelBtn->setEnabled(false);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void WarringDialog::on_cancelBtn_clicked()
{
    this->reject();
    close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void WarringDialog::on_okBtn_clicked()
{
    this->accept();
    close();
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void
*/
void WarringDialog::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            break;
        }
        default:
            break;
        }
        QDialog::changeEvent(event);
    }
}
