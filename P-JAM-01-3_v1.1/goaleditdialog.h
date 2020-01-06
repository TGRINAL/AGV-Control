/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            goalEditDialog.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#ifndef GOALEDITDIALOG_H
#define GOALEDITDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QQuaternion>
#include <QtMath>
#include <QGraphicsPixmapItem>

namespace Ui {
class goalEditDialog;
}

/*
*Class Description
*/
class goalEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit goalEditDialog(int, QPointF, QQuaternion, float, float, float, QString, QWidget *parent = 0);
    goalEditDialog(int, QPointF, QQuaternion, QWidget *parent = 0);
    void setMapInfo(float, float, float, float, float);
    ~goalEditDialog();

signals:
    void updateGoalValue(int order, QPointF pos, float thetaValue, float disTolerance, float angleTolerance, float reachTolerance, QString remark);
    void cloneGoal(int, QPointF pos, float thetaValue, float disTolerance, float angleTolerance, float reachTolerance, QString remark);
    void addCorrectPoint(QPointF, QQuaternion, int);
    void updateCorrectPoint(int order, QPointF pos, float thetaValue);

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();
    void on_coordinateMapBtn_clicked();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::goalEditDialog *ui;
    int goalOrder;
    QString goalRemark;
    bool isEditCorrectPoint;
    QList<float> valueList;
};

#endif // GOALEDITDIALOG_H
