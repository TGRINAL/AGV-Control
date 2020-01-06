/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            goalEditDialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "goaleditdialog.h"
#include "ui_goaleditdialog.h"

goalEditDialog::goalEditDialog(int order, QPointF pos, QQuaternion quat, float disTolerance, float angleTolerance, float reachTolerance, QString remark, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::goalEditDialog),
    isEditCorrectPoint(false)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    goalOrder = order;
    goalRemark = remark;
    ui->xDoubleSpinBox->setValue(pos.y());
    ui->yDoubleSpinBox->setValue(pos.x());
    ui->orientationDoubleSpinBox->setValue(quat.toEulerAngles().z());
    ui->disToleranceDSB->setValue(disTolerance);
    ui->angleToleranceDSB->setValue(angleTolerance);
    ui->reachToleranceDSB->setValue(reachTolerance);
    ui->remarkLineEdit->setText(remark);

    valueList.clear();
    valueList.append((float)ui->xDoubleSpinBox->value());
    valueList.append((float)ui->yDoubleSpinBox->value());
    valueList.append((float)ui->orientationDoubleSpinBox->value());
    valueList.append((float)ui->disToleranceDSB->value());
    valueList.append((float)ui->angleToleranceDSB->value());
    valueList.append((float)ui->reachToleranceDSB->value());

    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(QPixmap(":/xy_description.png"));
    double ratio = (double) (241.0-10)/pixmapItem->pixmap().height();
    pixmapItem->setScale(ratio);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addItem(pixmapItem);
    ui->xyGraphicsView->setScene(scene);

    ui->xyGraphicsView->hide();
    this->setFixedWidth(340);
}

goalEditDialog::goalEditDialog(int order, QPointF pos, QQuaternion quat, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::goalEditDialog),
    isEditCorrectPoint(true)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    goalOrder = order;
    ui->xDoubleSpinBox->setValue(pos.y());
    ui->yDoubleSpinBox->setValue(pos.x());
    ui->orientationDoubleSpinBox->setValue(quat.toEulerAngles().z());

    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(QPixmap(":/xy_description.png"));
    double ratio = (double) (241.0-10)/pixmapItem->pixmap().height();
    pixmapItem->setScale(ratio);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addItem(pixmapItem);
    ui->xyGraphicsView->setScene(scene);

    ui->disToleranceLabel->setVisible(false);
    ui->disToleranceDSB->setVisible(false);
    ui->angleToleranceLabel->setVisible(false);
    ui->angleToleranceDSB->setVisible(false);
    ui->reachToleranceLabel->setVisible(false);
    ui->reachToleranceDSB->setVisible(false);
    ui->remarkLabel->setVisible(false);
    ui->remarkLineEdit->setVisible(false);
    ui->line->setVisible(false);
    this->setWindowTitle(tr("Correct Point Edit"));

    ui->xyGraphicsView->hide();
    this->setFixedHeight(160);
    this->setFixedWidth(340);
}

/*
*Function Description :
*Parameters : 	resolution, width, height, width_ori, height_ori
*Returns : 		void
*/
void goalEditDialog::setMapInfo(float resolution, float width, float height, float width_ori, float height_ori)
{
    ui->xDoubleSpinBox->setMaximum(height * resolution + height_ori);
    ui->xDoubleSpinBox->setMinimum(height_ori);

    ui->yDoubleSpinBox->setMaximum(width * resolution + width_ori);
    ui->yDoubleSpinBox->setMinimum(width_ori);
}

goalEditDialog::~goalEditDialog()
{
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void goalEditDialog::on_okBtn_clicked()
{
    if(isEditCorrectPoint){
        QPointF pos(ui->yDoubleSpinBox->value(), ui->xDoubleSpinBox->value());
        emit updateCorrectPoint(goalOrder, pos, qDegreesToRadians(ui->orientationDoubleSpinBox->value()));
    }
    else{
        if(valueList.at(0) != (float)ui->xDoubleSpinBox->value() || valueList.at(1) != (float)ui->yDoubleSpinBox->value()
                || valueList.at(2) != (float)ui->orientationDoubleSpinBox->value() || valueList.at(3) != (float)ui->disToleranceDSB->value()
                || valueList.at(4) != (float)ui->angleToleranceDSB->value() || valueList.at(5) != (float)ui->reachToleranceDSB->value() ){
            QPointF pos(ui->yDoubleSpinBox->value(), ui->xDoubleSpinBox->value());
            emit updateGoalValue(goalOrder, pos, ui->orientationDoubleSpinBox->value(),
                                 ui->disToleranceDSB->value(), ui->angleToleranceDSB->value(), ui->reachToleranceDSB->value(), ui->remarkLineEdit->text());
        }
    }
    close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void goalEditDialog::on_cancelBtn_clicked()
{
    close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void 
*/
void goalEditDialog::on_coordinateMapBtn_clicked()
{
    if(ui->xyGraphicsView->isHidden())
    {
        ui->xyGraphicsView->show();
        this->setFixedWidth(590);
    }
    else
    {
        ui->xyGraphicsView->hide();
        this->setFixedWidth(340);
    }
}

/*
*Function Description :
*Parameters : 	event pointer
*Returns : 		void 
*/
void goalEditDialog::changeEvent(QEvent *event)
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
