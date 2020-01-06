/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            listdialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "listdialog.h"
#include "ui_listdialog.h"

ListDialog::ListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listDialog)
{
    ui->setupUi(this);
    ui->loadBtn->setEnabled(false);
    ui->deleteBtn->setEnabled(false);
}

ListDialog::~ListDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ListDialog::clearList()
{
    ui->listWidget->clear();
    ui->loadBtn->setEnabled(false);
}

/*
*Function Description :
*Parameters : 	strList
*Returns : 		void 
*/
void ListDialog::setList(QStringList strList)
{
    for(int i=0; i<strList.count(); i++){
        QString nameStr = strList.at(i);
        ui->listWidget->addItem(nameStr.split(".").first());
    }
}

/*
*Function Description :
*Parameters : 	listType
*Returns : 		void 
*/
void ListDialog::setListType(LIST_TYPE listType)
{
    m_listType = listType;
    if(m_listType == LIST_TYPE::LISTMAP)
        this->setWindowTitle(tr("Map List"));
    else if(m_listType == LIST_TYPE::LISTSCRIPT)
        this->setWindowTitle(tr("Script List"));
}

/*
*Function Description :
*Parameters : 	item pointer
*Returns : 		void 
*/
void ListDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(ui->listWidget->currentRow() == -1)
        return;
    if(m_listType == LIST_TYPE::LISTMAP)
        emit loadMap(ui->listWidget->currentItem()->text());
    else if(m_listType == LIST_TYPE::LISTSCRIPT)
        emit loadScript(ui->listWidget->currentItem()->text());
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ListDialog::on_loadBtn_clicked()
{
    if(ui->listWidget->currentRow() == -1)
        return;
    if(m_listType == LIST_TYPE::LISTMAP)
        emit loadMap(ui->listWidget->currentItem()->text());
    else if(m_listType == LIST_TYPE::LISTSCRIPT)
        emit loadScript(ui->listWidget->currentItem()->text());
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ListDialog::on_cancelBtn_clicked()
{
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ListDialog::on_listWidget_itemSelectionChanged()
{
    if(ui->listWidget->currentRow() != -1){
        ui->loadBtn->setEnabled(true);
        ui->deleteBtn->setEnabled(true);
    }
    else{
        ui->loadBtn->setEnabled(false);
        ui->deleteBtn->setEnabled(false);
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void ListDialog::on_deleteBtn_clicked()
{
    if(ui->listWidget->currentRow() == -1)
        return;

    QMessageBox *mb = new QMessageBox;
    mb->setIcon(QMessageBox::Question);
    mb->setText(tr("Warning"));
    mb->setInformativeText(tr("The delete action is unrecoverable, are you sure to execute it?"));
    mb->setStandardButtons(QMessageBox::Apply|QMessageBox::Cancel);
    mb->setButtonText(QMessageBox::Apply, tr("Apply"));
    mb->setButtonText(QMessageBox::Cancel, tr("Cancel"));
    mb->setDefaultButton(QMessageBox::Cancel);
    int ret = mb->exec();
    mb->deleteLater();
    switch(ret){
    case QMessageBox::Apply:
        if(m_listType == LIST_TYPE::LISTMAP)
            emit deleteMap(ui->listWidget->currentItem()->text());
        else if(m_listType == LIST_TYPE::LISTSCRIPT)
            emit deleteScript(ui->listWidget->currentItem()->text());
        delete ui->listWidget->takeItem(ui->listWidget->row(ui->listWidget->currentItem()));
        ui->listWidget->setCurrentRow(-1);
        break;
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void ListDialog::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
        }
        default:
            break;
        }
        QDialog::changeEvent(event);
    }
}
