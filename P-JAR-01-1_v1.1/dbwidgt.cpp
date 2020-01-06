/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            dbwidgt.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "dbwidgt.h"
#include "ui_dbwidgt.h"

DbWidgt::DbWidgt(DB_TYPE type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DbWidgt)
{
    ui->setupUi(this);
    m_type = type;
    model = new QSqlRelationalTableModel;
    initSqlTable(type);
    initModel();
    readDbFromFile();
    ui->acTableView->selectRow(0);
}

DbWidgt::~DbWidgt()
{
    writeDbToFile();
    delete model;
    delete ui;
}

/*
*Function Description :
*Parameters : 	type
*Returns : 		void 
*/
void DbWidgt::initSqlTable(DB_TYPE type)
{
    QSqlQuery query;
    switch (type) {
    case DB_TYPE::DESK:
        m_name = "DeskDatabase";
        query.exec("CREATE table " + m_name + " (id int primary key, number char(50), identifier char(50), script char(50))");
        break;
    case DB_TYPE::CARD:
        m_name = "CardDatabase";
        query.exec("CREATE table " + m_name + " (id int primary key, number char(50), identifier char(50))");
        break;
    default:
        break;
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::initModel()
{
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);                  // OnFieldChange, OnRowChange, OnManualSubmit
    model->setTable(m_name);
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Number"));
    model->setHeaderData(2, Qt::Horizontal, tr("Identifier"));
    if(m_type == DB_TYPE::DESK)
        model->setHeaderData(3, Qt::Horizontal, tr("Script"));
    model->setSort(0, Qt::AscendingOrder);
    model->select();

    ui->acTableView->setModel(model);
    ui->acTableView->hideColumn(0);
    ui->acTableView->setItemDelegate(new QSqlRelationalDelegate(ui->acTableView));
    if(m_type == DB_TYPE::DESK)
        for(int i=0; i<4; i++)
            ui->acTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    else
        for(int i=0; i<3; i++)
            ui->acTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    ui->acTableView->repaint();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::writeSettings()
{
    QSettings settings;
    settings.beginGroup("DeliveryDb");
    settings.beginWriteArray(m_name);
    for (int i = 0; i < getModel()->rowCount(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("number", getModel()->index(i, 1).data().toString());
        settings.setValue("identifier", getModel()->index(i, 2).data().toString());
    }
    settings.endArray();
    settings.endGroup();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::readSettings()
{
    QSettings settings;
    settings.beginGroup("DeliveryDb");
    int size = settings.beginReadArray(m_name);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString number = settings.value("number").toString();
        QString identifier = settings.value("identifier").toString();
        addDbFinish(number, identifier);
    }
    settings.endArray();
    settings.endGroup();
    getModel()->submitAll();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::writeDbToFile()
{
    QDir dir = QDir::currentPath();
    if(dir.exists("ur.exe"))
        dir.cdUp();
    QDir::setCurrent(dir.path());

    QString saveFileName = QString("%1.txt").arg(m_name);
    QFile file(saveFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return ;
    QTextStream outStream(&file);
    if(m_type == DB_TYPE::DESK)
        for(int i=0; i< getModel()->rowCount(); i++){
            outStream << getModel()->index(i, 1).data().toString() << " "
                      << getModel()->index(i, 2).data().toString() << " "
                      << getModel()->index(i, 3).data().toString() << endl;
        }
    else
        for(int i=0; i< getModel()->rowCount(); i++){
            outStream << getModel()->index(i, 1).data().toString() << " "
                      << getModel()->index(i, 2).data().toString() << endl;
        }
    file.close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::readDbFromFile()
{
    QDir dir = QDir::currentPath();
    if(dir.exists("ur.exe"))
        dir.cdUp();
    QDir::setCurrent(dir.path());

    QString loadFileName = QString("%1.txt").arg(m_name);
    QFile file(loadFileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return ;
    QTextStream inStream(&file);
    while(!inStream.atEnd()) {
        QString line = inStream.readLine();
        QStringList fields = line.split(" ");
        if(m_type == DB_TYPE::DESK)
            addDbFinish(fields.at(0), fields.at(1), fields.at(2));
        else
            addDbFinish(fields.at(0), fields.at(1));
        //qDebug() << line << fields;
    }
    file.close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_addButton_clicked()
{
    EditDbDialog *dlg = new EditDbDialog(true, m_type);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    switch (m_type) {
    case DB_TYPE::DESK:
        dlg->setWindowTitle("Edit Desk");
        break;
    case DB_TYPE::CARD:
        dlg->setWindowTitle("Edit Card");
        break;
    default:
        break;
    }

    connect(dlg, SIGNAL(addDbFinish(QString,QString)), this, SLOT(addDbFinish(QString,QString)));

    dlg->setModal(true);
    dlg->move(QCursor::pos().x()-dlg->size().width()/2, QCursor::pos().y()-dlg->size().height()/2);
    dlg->show();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_editBtn_clicked()
{
    int currentRow = ui->acTableView->currentIndex().row();
    if(currentRow < 0)
        return;

    if(ui->acTableView->currentIndex().column() < 3){
        EditDbDialog *dlg
                = new EditDbDialog(false, m_type, ui->acTableView->currentIndex().model()->index(currentRow, 1).data().toString(),
                                   ui->acTableView->currentIndex().model()->index(currentRow, 2).data().toString());
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        currentEditNum = ui->acTableView->currentIndex().model()->index(currentRow, 1).data().toString();
        switch (m_type) {
        case DB_TYPE::DESK:
            dlg->setWindowTitle("Edit Desk");
            break;
        case DB_TYPE::CARD:
            dlg->setWindowTitle("Edit Card");
            break;
        default:
            break;
        }
        connect(dlg, &EditDbDialog::editDbFinish, this, &DbWidgt::editDbFinish);
        dlg->setModal(true);
        dlg->move(QCursor::pos().x()-dlg->size().width()/2, QCursor::pos().y()-dlg->size().height()/2);
        dlg->show();
    }
    else if(ui->acTableView->currentIndex().column() == 3){
        emit deskAskListScript();
    }
}

/*
*Function Description :
*Parameters : 	&index
*Returns : 		void 
*/
void DbWidgt::on_acTableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_editBtn_clicked();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_deletehButton_clicked()
{
    int idx = ui->acTableView->currentIndex().row();
    if(idx < 0)
        return;
    QSqlQuery query;
    query.prepare("DELETE FROM " + m_name + " WHERE id = ?");
    query.addBindValue(1+idx);
    query.exec();

    //update id key order
    query.exec("SELECT id FROM " + m_name);
    int cnt= 1, id;
    while (query.next()){
        id = query.value(0).toInt();
        query.exec(" UPDATE " + m_name +
                   " SET id = " + QString::number(cnt) +
                   " WHERE id = " + QString::number(id));
        cnt++;
        query.exec("SELECT id FROM " + m_name + " WHERE id > " + QString::number(id));
    }
    model->submitAll();
    initModel();

    if(idx > ui->acTableView->model()->rowCount()-1)
        ui->acTableView->selectRow(ui->acTableView->model()->rowCount()-1);
    else
        ui->acTableView->selectRow(idx);
}

/*
*Function Description :
*Parameters : 	numberStr, idStr
*Returns : 		void 
*/
void DbWidgt::addDbFinish(QString numberStr, QString idStr)
{
    for(int i=0; i< model->rowCount();i++){
        if(model->index(i,1).data().toString() == numberStr.remove(QChar(' '), Qt::CaseInsensitive)){
            switch (m_type) {
            case DB_TYPE::DESK:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate desk number!"));
                break;
            case DB_TYPE::CARD:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate card number!"));
                break;
            default:
                break;
            }
            return;
        }
    }

    QSqlQuery query;
    query.exec("SELECT id FROM " + m_name);
    QString lastID;
    while (query.next())
        lastID = query.value(0).toString();
    if(m_type == DB_TYPE::DESK){
        query.prepare("INSERT INTO " + m_name + " (id, number, identifier, script) VALUES (:id, :number, :identifier, :script)");
        query.bindValue(":id", lastID.toInt()+1);
        query.bindValue(":number", numberStr.toLatin1());
        query.bindValue(":identifier", idStr.toLatin1());
        query.bindValue(":script", " ");
    }
    else{
        query.prepare("INSERT INTO " + m_name + " (id, number, identifier) VALUES (:id, :number, :identifier)");
        query.bindValue(":id", lastID.toInt()+1);
        query.bindValue(":number", numberStr.toLatin1());
        query.bindValue(":identifier", idStr.toLatin1());
    }
    query.exec();

    model->submitAll();
    initModel();
    ui->acTableView->selectRow(ui->acTableView->model()->rowCount()-1);
}

/*
*Function Description :
*Parameters : 	numberStr, idStr, scriptStr
*Returns : 		void 
*/
void DbWidgt::addDbFinish(QString numberStr, QString idStr, QString scriptStr)
{
    for(int i=0; i< model->rowCount();i++){
        if(model->index(i,1).data().toString() == numberStr.remove(QChar(' '), Qt::CaseInsensitive)){
            switch (m_type) {
            case DB_TYPE::DESK:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate desk number!"));
                break;
            case DB_TYPE::CARD:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate card number!"));
                break;
            default:
                break;
            }
            return;
        }
    }

    if(scriptStr.isEmpty())
        scriptStr = " ";

    QSqlQuery query;
    query.exec("SELECT id FROM " + m_name);
    QString lastID;
    while (query.next())
        lastID = query.value(0).toString();
    if(m_type == DB_TYPE::DESK){
        query.prepare("INSERT INTO " + m_name + " (id, number, identifier, script) VALUES (:id, :number, :identifier, :script)");
        query.bindValue(":id", lastID.toInt()+1);
        query.bindValue(":number", numberStr.toLatin1());
        query.bindValue(":identifier", idStr.toLatin1());
        query.bindValue(":script", scriptStr.toLatin1());
    }
    else{
        query.prepare("INSERT INTO " + m_name + " (id, number, identifier) VALUES (:id, :number, :identifier)");
        query.bindValue(":id", lastID.toInt()+1);
        query.bindValue(":number", numberStr.toLatin1());
        query.bindValue(":identifier", idStr.toLatin1());
    }
    query.exec();

    model->submitAll();
    initModel();
    ui->acTableView->selectRow(ui->acTableView->model()->rowCount()-1);
}

/*
*Function Description :
*Parameters : 	numberStr, idStr
*Returns : 		void 
*/
void DbWidgt::editDbFinish(QString numberStr, QString idStr)
{
    for(int i=0; i< model->rowCount();i++){
        if(model->index(i,1).data().toString() == numberStr.remove(QChar(' '), Qt::CaseInsensitive)){
            if(currentEditNum == model->index(i,1).data().toString())
                break;
            switch (m_type) {
            case DB_TYPE::DESK:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate desk number!"));
                break;
            case DB_TYPE::CARD:
                QMessageBox::warning(this, tr("Error"), tr("Duplicate card number!"));
                break;
            default:
                break;
            }
            return;
        }
    }

    int currentRow = ui->acTableView->currentIndex().row();
    QSqlQuery query;
    query.exec( " UPDATE " + m_name +
                " SET number = '" + numberStr + "' " +
                ", identifier = '" + idStr + "' " +
                " WHERE id = " + QString::number(currentRow+1));
    model->submitAll();
    initModel();
    ui->acTableView->selectRow(currentRow);
}

/*
*Function Description :
*Parameters : 	id
*Returns : 		void 
*/
void DbWidgt::addEmptyDb(QString id)
{
    EditDbDialog *dlg = new EditDbDialog(true, m_type, id);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    switch (m_type) {
    case DB_TYPE::DESK:
        dlg->setWindowTitle("Edit Desk");
        break;
    case DB_TYPE::CARD:
        dlg->setWindowTitle("Edit Card");
        break;
    default:
        break;
    }

    connect(dlg, SIGNAL(addDbFinish(QString,QString)), this, SLOT(addDbFinish(QString,QString)));
    dlg->setModal(true);
    dlg->move(QCursor::pos().x()-dlg->size().width()/2, QCursor::pos().y()-dlg->size().height()/2);
    dlg->show();
}

/*
*Function Description :
*Parameters : 	scriptName
*Returns : 		void 
*/
void DbWidgt::deskGetScriptName(QString scriptName)
{
    int currentRow = ui->acTableView->currentIndex().row();
    qDebug() << scriptName << currentRow;

    QSqlQuery query;
    query.exec( " UPDATE " + m_name +
                " SET script = '" + scriptName + "' " +
                " WHERE id = " + QString::number(currentRow+1));
    model->submitAll();
    initModel();
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void DbWidgt::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            initModel();
            break;
        }
        default:
            break;
        }
        QWidget::changeEvent(event);
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_logoutButton_clicked()
{
    writeDbToFile();
    close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_upBtn_clicked()
{
    int idx = ui->acTableView->currentIndex().row();
    if(!idx)
        return;

    QSqlQuery query;
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(99) +
               " WHERE id = " + QString::number(idx+1));
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(idx+1) +
               " WHERE id = " + QString::number(idx));
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(idx) +
               " WHERE id = " + QString::number(99));

    model->submitAll();
    initModel();
    ui->acTableView->selectRow(idx-1);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_downBtn_clicked()
{
    int idx = ui->acTableView->currentIndex().row();
    if(idx == ui->acTableView->model()->rowCount()-1)
        return;
    QSqlQuery query;
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(99) +
               " WHERE id = " + QString::number(idx+2));
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(idx+2) +
               " WHERE id = " + QString::number(idx+1));
    query.exec(" UPDATE " + m_name +
               " SET id = " + QString::number(idx+1) +
               " WHERE id = " + QString::number(99));

    model->submitAll();
    initModel();
    ui->acTableView->selectRow(idx+1);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void DbWidgt::on_joystickAdminBtn_clicked()
{
    emit joystick4Administrator();
}
