/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            batteryitem.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef BATTERYITEM_H
#define BATTERYITEM_H

#include <QWidget>
#include "ui_batteryitem.h"

/*
*Class Description
*/
class BatteryItem : public QWidget
{
    Q_OBJECT

public:
    BatteryItem(QWidget *parent = 0);
    BatteryItem(QWidget *parent, int value, int color = 0);
    void setBatteryValue(int value);
    void setBatteryColor(int color);
    void setShowText(int flg=0, QString str="");
    ~BatteryItem();

private:
    Ui::BatteryItem ui;
    int m_value;
    QColor m_color = QColor(0,255,0);
    int m_showTextFlg = 0;
    QString m_showText;

protected:
    virtual void paintEvent(QPaintEvent * event) override;

};

#endif // BATTERYITEM_H
