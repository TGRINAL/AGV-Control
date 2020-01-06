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

EditDbDialog::EditDbDialog(bool flag, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->numberLineEdit->setText("");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText("");
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::EditDbDialog(bool flag, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->numberLineEdit->setText("");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::EditDbDialog(bool flag, QString num, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->numberLineEdit->setText(num);
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::~EditDbDialog()
{
    delete ui;
}
void EditDbDialog::on_okBtn_clicked()
{
    if(ui->idLineEdit->text().isEmpty() || ui->numberLineEdit->text().isEmpty()){
        QMessageBox::critical(this, tr("Error"), "Empty information");
        return;
    }
    if(isAddDb)
        emit addDbFinish(ui->numberLineEdit->text(), ui->idLineEdit->text());
    else
        emit editDbFinish(ui->numberLineEdit->text(), ui->idLineEdit->text());
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
