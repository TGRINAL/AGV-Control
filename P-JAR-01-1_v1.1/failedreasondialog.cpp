/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            failedreasondialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "failedreasondialog.h"
#include "ui_failedreasondialog.h"

FailedReasonDialog::FailedReasonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FailedReasonDialog)
{
    ui->setupUi(this);
}

FailedReasonDialog::~FailedReasonDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void FailedReasonDialog::on_okBtn_clicked()
{
    FAILED_REASON failedType = FAILED_REASON::ChannelObstacle;
    if(ui->radioButton_1->isChecked()){
        failedType = FAILED_REASON::ChannelObstacle;
    }
    else if(ui->radioButton_2->isChecked()){
        failedType = FAILED_REASON::FieldChange;
    }
    else if(ui->radioButton_3->isChecked()){
        failedType = FAILED_REASON::GetLost;
    }
    else if(ui->radioButton_4->isChecked()){
        failedType = FAILED_REASON::ArmError;
    }
    else if(ui->radioButton_5->isChecked()){
        failedType = FAILED_REASON::MealError;
    }
    else if(ui->radioButton_6->isChecked()){
        failedType = FAILED_REASON::NotGoing;
    }
    else if(ui->radioButton_7->isChecked()){
        failedType = FAILED_REASON::Other;
    }
    else if(ui->radioButton_8->isChecked()){
        failedType = FAILED_REASON::EngineeringTest;
    }
    this->hide();
    emit parseFailedReason(failedType);
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void FailedReasonDialog::on_cancelBtn_clicked()
{
    this->close();
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void FailedReasonDialog::changeEvent(QEvent *event)
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
