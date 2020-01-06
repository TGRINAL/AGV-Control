/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            main.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>

#include "mywidget.h"
#include "initdb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //for QSetting
    QCoreApplication::setOrganizationName("TECO");
    QCoreApplication::setOrganizationDomain("teco.com.tw");
    QCoreApplication::setApplicationName("AGV");

    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        QMessageBox::critical(0, "Unable to initialize Database",
                              "Error initializing database: " + err.text());
        qCritical("Error initializing database");
    }

    QSharedMemory shared("62d60669-bb94-4a94-88bb-b964890a7e04");
    if( !shared.create( 512, QSharedMemory::ReadWrite) )
    {
        qWarning() << "Can't start more than one instance of the application.";
        exit(0);
    }
    else {
        qDebug() << "Application started successfully.";
    }

    MyWidget w;
    w.showMaximized();
    w.show();

    return a.exec();
}
