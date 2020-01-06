/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            joystickdialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "joystickdialog.h"
#include "ui_joystickdialog.h"

JoystickDialog::JoystickDialog(bool flicker, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoystickDialog),
    flickerFlag(false)
{
    ui->setupUi(this);
    teleopMode = TELEOP_TYPE::LINEAR;
    ui->linearRb->setChecked(true);
    flickerFlag = flicker;

    backgroundTimer = new QTimer;
    backgroundTimer->setInterval(1000);
    connect(backgroundTimer, &QTimer::timeout, this, &JoystickDialog::handleBackgroundTimeout);
    if(flickerFlag)
        backgroundTimer->start();
    else{
        backgroundTimer->stop();
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");

    }
    ui->modeGb->setVisible(false);
    ui->linearRb->setVisible(false);
    ui->jogRb->setVisible(false);
}

JoystickDialog::~JoystickDialog()
{
    emit setTeleopMode(teleopMode);
    emit setKeyPriorityFlag(true);
    delete backgroundTimer;
    delete ui;
}

/*
*Function Description :
*Parameters : 	flag
*Returns : 		void 
*/
void JoystickDialog::setFlicker(bool flag)
{
    flickerFlag = flag;
    if(flickerFlag)
        backgroundTimer->start();
    else{
        backgroundTimer->stop();
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::handleBackgroundTimeout()
{
    static bool pingpong = true;
    if(pingpong)
        this->setStyleSheet("QDialog{background:rgba(255,0,0,255);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    else
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    pingpong = !pingpong;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_closeBtn_clicked()
{
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_forwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Forward);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_backwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Backward);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_leftBtn_pressed()
{
    emit updateVel(VEL_TYPE::Left);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_rightBtn_pressed()
{
    emit updateVel(VEL_TYPE::Right);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_forwardBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_backwardBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_leftBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_rightBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_stopBtn_clicked()
{
    emit updateVel(VEL_TYPE::Stop);
}

/*
*Function Description :
*Parameters : 	checked
*Returns : 		void 
*/
void JoystickDialog::on_jogRb_toggled(bool checked)
{
    if(checked)
        teleopMode = TELEOP_TYPE::JOG;
    else
        teleopMode = TELEOP_TYPE::LINEAR;
    emit updateVel(VEL_TYPE::Stop);
    emit setTeleopMode(teleopMode);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_abortBtn_clicked()
{
    emit abortMovebase();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_goOriginBtn_clicked()
{
    emit goOrigin();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_resetOriginBtn_clicked()
{
    emit resetOrigin();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void JoystickDialog::on_safeBtn_clicked()
{
    if(ui->safeBtn->text() == tr("Safe Stop")){
        ui->safeBtn->setText(tr("No safe stop"));
        ui->safeBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
        emit setKeyPriorityFlag(false);
    }
    else if(ui->safeBtn->text() == tr("No safe stop")){
        ui->safeBtn->setText(tr("Safe Stop"));
        ui->safeBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        emit setKeyPriorityFlag(true);
    }
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void JoystickDialog::changeEvent(QEvent *event)
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
