/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            resetorigindialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/
			
#ifndef RESETORIGINDIALOG_H
#define RESETORIGINDIALOG_H

#include <QDialog>

#include "systemsettingdialog.h"

enum ORIGINMODE {RESETORIGIN = 60, STARTORIGIN};

namespace Ui {
class ResetOriginDialog;
}

/*
* Class Description
*/
class ResetOriginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetOriginDialog(ORIGINMODE mode, QList<Origin *>, QWidget *parent = nullptr);
    ResetOriginDialog(ORIGINMODE mode, QList<ScriptButton *>, QWidget *parent = nullptr);
    ~ResetOriginDialog();

private:
    Ui::ResetOriginDialog *ui;
    ORIGINMODE m_mode;

signals:
    void resetOrigin(int);
    void poseUpdte();
    void startFromWhere(int);
    void startScript(int);

private slots:
    //void on_resetOriginBtn1_clicked();
    //void on_resetOriginBtn2_clicked();
    //void on_resetOriginBtn3_clicked();
    void on_poseUpdateBtn_clicked();
    void on_closeBtn_clicked();

    void buttonClick();
};

#endif // RESETORIGINDIALOG_H
