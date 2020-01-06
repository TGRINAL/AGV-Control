/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            listdialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef ListDialog_H
#define ListDialog_H

#include <QDialog>
#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>

enum LIST_TYPE{LISTMAP, LISTSCRIPT};

namespace Ui {
class listDialog;
}

/*
* Class Description
*/
class ListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListDialog(QWidget *parent = 0);
    ~ListDialog();

    void clearList();
    void setList(QStringList);

public slots:
    void setListType(LIST_TYPE listType);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_loadBtn_clicked();
    void on_cancelBtn_clicked();
    void on_listWidget_itemSelectionChanged();
    void on_deleteBtn_clicked();

signals:
    void loadMap(QString);
    void loadScript(QString);
    void deleteMap(QString);
    void deleteScript(QString);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::listDialog *ui;
    LIST_TYPE m_listType;
};

#endif // ListDialog_H
