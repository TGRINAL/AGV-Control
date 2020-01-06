/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            dbwidgt.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef DBWIDGT_H
#define DBWIDGT_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QMessageBox>
#include <QSqlQuery>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "editdbdialog.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QSqlRelationalTableModel;
QT_END_NAMESPACE

namespace Ui {
class DbWidgt;
}

/*
* Class Description
*/
class DbWidgt : public QWidget
{
    Q_OBJECT

public:
    explicit DbWidgt(DB_TYPE, QWidget *parent = 0);
    ~DbWidgt();
    QSqlRelationalTableModel *getModel() { return model;}
    void initSqlTable(DB_TYPE);
    void initModel();
    void writeSettings();
    void readSettings();
    void writeDbToFile();
    void readDbFromFile();
    void addDbFinish(QString, QString, QString);

private slots:
    void on_addButton_clicked();
    void on_deletehButton_clicked();
    void on_editBtn_clicked();
    void on_acTableView_doubleClicked(const QModelIndex &index);
    void on_logoutButton_clicked();
    void on_upBtn_clicked();
    void on_downBtn_clicked();
    void on_joystickAdminBtn_clicked();

public slots:
    void addDbFinish(QString, QString);
    void editDbFinish(QString, QString);
    void addEmptyDb(QString);
    void deskGetScriptName(QString);

signals:
    void deskAskListScript();
    void joystick4Administrator();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::DbWidgt *ui;
    QSqlRelationalTableModel *model;
    DB_TYPE m_type;
    QString m_name;
    QString currentEditNum;
};

#endif // DBWIDGT_H
