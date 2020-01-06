/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            changepwddialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "changepwddialog.h"
#include "ui_changepwddialog.h"

ChangePwdDialog::ChangePwdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePwdDialog),
    isOnePasswordMode(false)
{
    ui->setupUi(this);
}

ChangePwdDialog::~ChangePwdDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	parent pointer, ok pointer
*Returns : 		QString 
*/
QString ChangePwdDialog::getOnePassword(QWidget *parent, bool *ok)
{
    ChangePwdDialog *dialog = new ChangePwdDialog(parent);
    dialog->setOnePasswordMode();
    dialog->setModal(true);
    dialog->setFixedSize(dialog->width(), dialog->height()*2/3);

    const int ret = dialog->exec();
    if(ok)
        *ok = !!ret;
    if(ret)
        return dialog->ui->pwdLineEdit_1->text();
    else
        return QString();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ChangePwdDialog::on_cancelBtn_clicked()
{
    this->done(QDialog::Rejected);
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ChangePwdDialog::on_confirmBtn_clicked()
{
    if(!isOnePasswordMode){
        if(!ui->pwdLineEdit_1->text().isEmpty() && !ui->pwdLineEdit_2->text().isEmpty()
                && ui->pwdLineEdit_1->text() == ui->pwdLineEdit_2->text()
                && ui->pwdLineEdit_1->text().length() >= 4 && ui->pwdLineEdit_2->text().length() >= 4){
            emit setNewPassword(ui->pwdLineEdit_1->text());
            QMessageBox::information(this, tr("OK"), tr("Change password OK!"));
            this->done(QDialog::Accepted);
            this->close();
        }
        else{
            if(ui->pwdLineEdit_1->text() != ui->pwdLineEdit_2->text())
                QMessageBox::warning(this, tr("Error"), tr("Two input passwords are different!"));
            else if(ui->pwdLineEdit_1->text().isEmpty() || ui->pwdLineEdit_2->text().isEmpty())
                QMessageBox::warning(this, tr("Error"), tr("Empty password!"));
            else if(ui->pwdLineEdit_1->text().length() < 4 || ui->pwdLineEdit_2->text().length() < 4)
                QMessageBox::warning(this, tr("Error"), tr("Password length needs at least 4!"));
        }
    }
    else if (isOnePasswordMode){
        this->done(QDialog::Accepted);
        this->close();
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ChangePwdDialog::setOnePasswordMode()
{
    isOnePasswordMode = true;
    this->setWindowTitle(tr("Enter Password"));
    ui->label_1->setText(tr("Password"));
    ui->label_2->setVisible(false);
    ui->pwdLineEdit_2->setVisible(false);
}

/*
*Function Description :
*Parameters : 	checked
*Returns : 		void 
*/
void ChangePwdDialog::on_showPwCheckBox_toggled(bool checked)
{
    if(checked){
        ui->pwdLineEdit_1->setEchoMode(QLineEdit::Normal);
        ui->pwdLineEdit_2->setEchoMode(QLineEdit::Normal);
    }
    else{
        ui->pwdLineEdit_1->setEchoMode(QLineEdit::Password);
        ui->pwdLineEdit_2->setEchoMode(QLineEdit::Password);
    }
}
