/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            editdbdialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "editdbdialog.h"
#include "ui_editdbdialog.h"

EditDbDialog::EditDbDialog(bool flag, DB_TYPE type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->cardNumLineEdit->setInputMask("999");
    ui->cardNumLineEdit->setText("");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText("");
    isAddDb = flag;
    ui->okBtn->setFocus();
    m_type = type;
    if(type == DB_TYPE::CARD){
        ui->deskNumLabel->setVisible(false);
        ui->deskNumComboBox->setVisible(false);
    }
    else if(type == DB_TYPE::DESK){
        ui->cardNumLabel->setVisible(false);
        ui->cardNumLineEdit->setVisible(false);
    }
}

EditDbDialog::EditDbDialog(bool flag, DB_TYPE type, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->cardNumLineEdit->setInputMask("999");
    ui->cardNumLineEdit->setText("");
    ui->cardNumLineEdit->setSelection(0, 3);
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    m_type = type;
    if(type == DB_TYPE::CARD){
        ui->deskNumLabel->setVisible(false);
        ui->deskNumComboBox->setVisible(false);
    }
    else if(type == DB_TYPE::DESK){
        ui->cardNumLabel->setVisible(false);
        ui->cardNumLineEdit->setVisible(false);
    }
}

EditDbDialog::EditDbDialog(bool flag, DB_TYPE type, QString num, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->cardNumLineEdit->setInputMask("999");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    ui->okBtn->setFocus();
    m_type = type;
    if(type == DB_TYPE::CARD){
        ui->cardNumLineEdit->setText(num);
        ui->deskNumLabel->setVisible(false);
        ui->deskNumComboBox->setVisible(false);
    }
    else if(type == DB_TYPE::DESK){
        ui->deskNumComboBox->setCurrentText(num);
        ui->cardNumLabel->setVisible(false);
        ui->cardNumLineEdit->setVisible(false);
    }
}

EditDbDialog::~EditDbDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void EditDbDialog::on_okBtn_clicked()
{
    QString numStr = "";
    if(m_type == DB_TYPE::CARD)
        numStr = ui->cardNumLineEdit->text();
    else if(m_type == DB_TYPE::DESK)
        numStr = ui->deskNumComboBox->currentText();

    if(ui->idLineEdit->text().isEmpty() || numStr.isEmpty()){
        QMessageBox::critical(this, tr("Error"), "Empty information");
        return;
    }

    if(isAddDb)
        emit addDbFinish(numStr, ui->idLineEdit->text());
    else
        emit editDbFinish(numStr, ui->idLineEdit->text());
    close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void EditDbDialog::on_cancelBtn_clicked()
{
    close();
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void EditDbDialog::changeEvent(QEvent *event)
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
