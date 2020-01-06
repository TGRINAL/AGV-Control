/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            actiondialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/

#include "actiondialog.h"
#include "ui_actiondialog.h"

ActionDialog::ActionDialog(bool isAgvOnly, bool isActionSelected, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionDialog),
    isEditAction(false),
    allCheckFlag(false)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    ui->glWeightScrollBar->setValue(50);
    ui->glSpinBox->setValue(50);
    if(isActionSelected)
        ui->add2BelowBtn->setEnabled(true);
    else
        ui->add2BelowBtn->setEnabled(false);

    ui->writeExtAddrSpinBox->setPrefix("0x");
    ui->writeExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->writeExtAddrSpinBox->setRange(0, 0x64);

    ui->writeExtValueSpinBox->setPrefix("0x");
    ui->writeExtValueSpinBox->setDisplayIntegerBase(16);
    ui->writeExtValueSpinBox->setRange(0, 0xFFFF);

    ui->requestExtAddrSpinBox->setPrefix("0x");
    ui->requestExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->requestExtAddrSpinBox->setRange(0, 0x64);

    ui->requestExtValueSpinBox->setPrefix("0x");
    ui->requestExtValueSpinBox->setDisplayIntegerBase(16);
    ui->requestExtValueSpinBox->setRange(0, 0xFFFF);

    setSomeWidgetInvisable();
    if(isAgvOnly)
        ui->agvRadioButton->setChecked(true);
    else
        ui->serviceRobotRadioButton->setChecked(true);
    setActionWidget(isAgvOnly);
}

ActionDialog::ActionDialog(bool isAgvOnly, int order, SUBACTION_TYPE type, int value, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionDialog),
    isEditAction(true),
    allCheckFlag(false)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    ui->writeExtAddrSpinBox->setPrefix("0x");
    ui->writeExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->writeExtAddrSpinBox->setRange(0, 0x64);

    ui->writeExtValueSpinBox->setPrefix("0x");
    ui->writeExtValueSpinBox->setDisplayIntegerBase(16);
    ui->writeExtValueSpinBox->setRange(0, 0xFFFF);

    ui->requestExtAddrSpinBox->setPrefix("0x");
    ui->requestExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->requestExtAddrSpinBox->setRange(0, 0x64);

    ui->requestExtValueSpinBox->setPrefix("0x");
    ui->requestExtValueSpinBox->setDisplayIntegerBase(16);
    ui->requestExtValueSpinBox->setRange(0, 0xFFFF);

    ui->add2EndBtn->setText(tr("Confirm"));
    ui->add2BelowBtn->setVisible(false);
    actionOrder = order;
    switch (type) {
    case SUBACTION_TYPE::AUDIO:
        ui->audioRadioBtn->setChecked(true);
        if(value > 9)  //thanks for coming, audio 1~9
            ui->audioComboBox->setCurrentIndex(value-2);
        else
            ui->audioComboBox->setCurrentIndex(value-1);
        break;
    case SUBACTION_TYPE::DELAY:
        ui->delayRadioBtn->setChecked(true);
        ui->delaySpinBox->setValue(value);
        break;
    case SUBACTION_TYPE::ARM:
        ui->armRadioBtn->setChecked(true);
        break;
    case SUBACTION_TYPE::SEARCHTAG:
        ui->searchTagRadioBtn->setChecked(true);
        ui->searchTagComboBox->setCurrentText(QString("%1").arg(value));
        break;
    case SUBACTION_TYPE::LED:
        ui->ledRadioBtn->setChecked(true);
        if(value == 99) //empty
            ui->ledComboBox->setCurrentIndex(4);
        else if(value == 11)    //Happy New Year
            ui->ledComboBox->setCurrentIndex(6);
        else if(value == 12)    //Kung Hei Fat Choi
            ui->ledComboBox->setCurrentIndex(7);
        else if(value > 20)     //for LED 1~6
            ui->ledComboBox->setCurrentIndex(value - 21 +5);
        else
            ui->ledComboBox->setCurrentIndex(value-1);
        break;
    default:
        break;
    }

    setSomeWidgetInvisable();
    if(isAgvOnly)
        ui->agvRadioButton->setChecked(true);
    else
        ui->serviceRobotRadioButton->setChecked(true);
    setActionWidget(isAgvOnly);

    cmpValueList.clear();
    cmpValueList.append((float)type);
    cmpValueList.append((float)value);
}

ActionDialog::ActionDialog(bool isAgvOnly, int order, SUBACTION_TYPE type, QList<float> valueList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionDialog),
    isEditAction(true),
    allCheckFlag(false)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    ui->writeExtAddrSpinBox->setPrefix("0x");
    ui->writeExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->writeExtAddrSpinBox->setRange(0, 0x64);

    ui->writeExtValueSpinBox->setPrefix("0x");
    ui->writeExtValueSpinBox->setDisplayIntegerBase(16);
    ui->writeExtValueSpinBox->setRange(0, 0xFFFF);

    ui->requestExtAddrSpinBox->setPrefix("0x");
    ui->requestExtAddrSpinBox->setDisplayIntegerBase(16);
    ui->requestExtAddrSpinBox->setRange(0, 0x64);

    ui->requestExtValueSpinBox->setPrefix("0x");
    ui->requestExtValueSpinBox->setDisplayIntegerBase(16);
    ui->requestExtValueSpinBox->setRange(0, 0xFFFF);

    ui->add2EndBtn->setText(tr("Confirm"));
    ui->add2BelowBtn->setVisible(false);

    actionOrder = order;
    switch (type) {
    case SUBACTION_TYPE::DYNPARAM:
        ui->dynparamRadioBtn->setChecked(true);
        ui->laserDistanceSpinBox->setValue(valueList.at(0));
        ui->laserDegreeMinSpinBox->setValue(valueList.at(1));
        ui->laserDegreeMaxSpinBox->setValue(valueList.at(2));
        if(valueList.count() < 3)
            break;
        ui->ginflationSpinBox->setValue(valueList.at(3));
        ui->linflationSpinBox->setValue(valueList.at(4));
        if(valueList.count() < 5)
            break;
        ui->gFootprintCB->setCurrentIndex((int)valueList.at(5));
        ui->lFootprintCB->setCurrentIndex((int)valueList.at(6));
        if(valueList.count() < 7)
            break;
        ui->glWeightScrollBar->setValue((int)valueList.at(7));
        ui->glSpinBox->setValue((int)valueList.at(7));
        if(valueList.count() < 8)
            break;
        ui->globalCombinMethodCB->setCurrentIndex((int)valueList.at(8));
        ui->localCombinMethodCB->setCurrentIndex((int)valueList.at(8));
        break;
    case SUBACTION_TYPE::IMAGEDETECT:{
        ui->imgDetcRadioButton->setChecked(true);
        int value = (int)valueList.at(0);

        if(((value&0x1)) == 0x1)
            ui->personCB->setChecked(true);
        else
            ui->personCB->setChecked(false);

        if(((value&0x2)) == 0x2)
            ui->chairCB->setChecked(true);
        else
            ui->chairCB->setChecked(false);

        if(((value&0x4)) == 0x4)
            ui->tableCB->setChecked(true);
        else
            ui->tableCB->setChecked(false);

        if(((value&0x8)) == 0x8)
            ui->otherCB->setChecked(true);
        else
            ui->otherCB->setChecked(false);
        break;
    }
    case SUBACTION_TYPE::REMARKS:
        ui->remarksRadioBtn->setChecked(true);
        ui->remarkComboBox->setCurrentIndex((int)valueList.at(0)-1);
        ui->remarkSubComboBox->setCurrentIndex((int)valueList.at(1)-1);
        break;
    case SUBACTION_TYPE::WRITEEXTCOMM:
        ui->writeExtCommRadioBtn->setChecked(true);
        ui->writeExtAddrSpinBox->setValue(valueList.at(0));
        ui->writeExtValueSpinBox->setValue(valueList.at(1));
        break;
    case SUBACTION_TYPE::REQUESTEXTCOMM:
        ui->requestExtCommRadioBtn->setChecked(true);
        ui->requestExtAddrSpinBox->setValue(valueList.at(0));
        ui->requestExtValueSpinBox->setValue(valueList.at(1));
        break;
    case SUBACTION_TYPE::COMBINETABLEDETECTION:
        ui->comTableDetecRadioBtn->setChecked(true);
        if((int)valueList.at(0) == 0){
            ui->jumpComboBox->setCurrentIndex(0);
            ui->jumpSpinBox->setValue(valueList.at(1));
        }
        else if ((int)valueList.at(0) == 1){
            ui->jumpComboBox->setCurrentIndex(1);
            ui->jumpRemarkComboBox->setCurrentIndex(valueList.at(1)-1);
            ui->jumpRemarkSubComboBox->setCurrentIndex(valueList.at(2)-1);
        }
        break;
    default:
        break;
    }

    setSomeWidgetInvisable();
    if(isAgvOnly)
        ui->agvRadioButton->setChecked(true);
    else
        ui->serviceRobotRadioButton->setChecked(true);
    setActionWidget(isAgvOnly);

    cmpValueList.clear();
    cmpValueList.append((float)type);
    for(int i=0; i<valueList.count(); i++)
        cmpValueList.append((float)valueList.at(i));
}

ActionDialog::~ActionDialog()
{
    emit setAgvOnlyFlag(ui->agvRadioButton->isChecked());
    delete ui;
}


/*
*Function Description :
*Parameters :   NONE
*Returns : 		void 
*/
void ActionDialog::on_cancelBtn_clicked()
{
    close();
}

/*
*Function Description :
*Parameters :   checked
*Returns : 		void 
*/
void ActionDialog::on_delayRadioBtn_toggled(bool checked)
{
    ui->delaySpinBox->setEnabled(checked);
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_audioComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->audioRadioBtn->setChecked(true);
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_searchTagComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->searchTagRadioBtn->setChecked(true);
}

/*
*Function Description :
*Parameters :   index
*Returns : 		void 
*/
void ActionDialog::on_ledComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->ledRadioBtn->setChecked(true);
}

/*
*Function Description :
*Parameters :   checked
*Returns : 		void 
*/
void ActionDialog::on_dynparamRadioBtn_toggled(bool checked)
{
    ui->laserDistanceSpinBox->setEnabled(checked);
    ui->laserDegreeMinSpinBox->setEnabled(checked);
    ui->laserDegreeMaxSpinBox->setEnabled(checked);
    ui->ginflationSpinBox->setEnabled(checked);
    ui->linflationSpinBox->setEnabled(checked);
    ui->gFootprintCB->setEnabled(checked);
    ui->lFootprintCB->setEnabled(checked);
    ui->glWeightScrollBar->setEnabled(checked);
    ui->glSpinBox->setEnabled(checked);
    ui->globalCombinMethodCB->setEnabled(checked);
    ui->localCombinMethodCB->setEnabled(checked);
}

/*
*Function Description :
*Parameters : value
*Returns : 		void 
*/
void ActionDialog::on_glWeightScrollBar_valueChanged(int value)
{
    float g = 65.0*(1-value/100.0);
    float l = 65.0*value/100.0;
    ui->LabelWeightingG->setText(QString("%1").arg(g, 0, 'f', 1));
    ui->LabelWeightingL->setText(QString("%1").arg(l, 0, 'f', 1));
    ui->glSpinBox->setValue(value);
}

/*
*Function Description :
*Parameters : value
*Returns : 	  void 
*/
void ActionDialog::on_glSpinBox_valueChanged(int value)
{
    ui->glWeightScrollBar->setValue(value);
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_imgDetcRadioButton_toggled(bool checked)
{
    ui->allCB->setEnabled(checked);
    ui->personCB->setEnabled(checked);
    ui->chairCB->setEnabled(checked);
    ui->tableCB->setEnabled(checked);
    ui->otherCB->setEnabled(checked);
}

/*
*Function Description :
*Parameters :   NONE
*Returns : 		void 
*/
void ActionDialog::setSomeWidgetInvisable()
{
    ui->searchTagRadioBtn->setVisible(false);
    ui->searchTagComboBox->setVisible(false);
    ui->imgDetcRadioButton->setVisible(false);
    ui->allCB->setVisible(false);
    ui->personCB->setVisible(false);
    ui->chairCB->setVisible(false);
    ui->tableCB->setVisible(false);
    ui->otherCB->setVisible(false);

    ui->horizontalLayout_footprint->setEnabled(false);
    ui->label_footprint->setVisible(false);
    ui->gFootprintLabel->setVisible(false);
    ui->gFootprintCB->setVisible(false);
    ui->lFootprintLabel->setVisible(false);
    ui->lFootprintCB->setVisible(false);

#ifndef SERVICE_ROBOT
    ui->armRadioBtn->setVisible(false);
    ui->audioRadioBtn->setVisible(false);
    ui->audioComboBox->setVisible(false);
    ui->ledRadioBtn->setVisible(false);
    ui->ledComboBox->setVisible(false);
    /*ui->dynparamRadioBtn->setVisible(false);
    ui->labelLaser->setVisible(false);
    ui->laserDegreeMaxSpinBox->setVisible(false);
    ui->laserDegreeMinSpinBox->setVisible(false);
    ui->laserDistanceSpinBox->setVisible(false);
    ui->labelLaserm->setVisible(false);
    ui->labelLaserWave->setVisible(false);
    ui->labelObstacle->setVisible(false);
    ui->labelObstacleG->setVisible(false);
    ui->labelObstacleL->setVisible(false);
    ui->globalCombinMethodCB->setVisible(false);
    ui->localCombinMethodCB->setVisible(false);
    ui->labelInflation->setVisible(false);
    ui->labelInflationG->setVisible(false);
    ui->labelInflationL->setVisible(false);
    ui->ginflationSpinBox->setVisible(false);
    ui->linflationSpinBox->setVisible(false);
    ui->labelGLWeight->setVisible(false);
    ui->LabelWeightingG->setVisible(false);
    ui->LabelWeightingL->setVisible(false);
    ui->glWeightScrollBar->setVisible(false);
    ui->glSpinBox->setVisible(false);*/
    this->setFixedHeight(500);
#endif
}

/*
*Function Description :
*Parameters : flag
*Returns : 		void 
*/
void ActionDialog::setActionWidget(bool flag)
{
    ui->armRadioBtn->setVisible(!flag);
    ui->audioRadioBtn->setVisible(!flag);
    ui->audioComboBox->setVisible(!flag);
    ui->ledRadioBtn->setVisible(!flag);
    ui->ledComboBox->setVisible(!flag);
    ui->comTableDetecRadioBtn->setVisible(!flag);
    ui->jumpComboBox->setVisible(!flag);
    ui->jumpSpinBox->setVisible(!flag);
    if(flag)
        this->setFixedHeight(500);
    else
        this->setFixedHeight(650);
    on_jumpComboBox_currentIndexChanged(ui->jumpComboBox->currentIndex());
}

/*
*Function Description :
*Parameters :  NONE
*Returns : 		void 
*/
void ActionDialog::on_add2EndBtn_clicked()
{
    QList<float> valueList;
    if(ui->armRadioBtn->isChecked())
        type = SUBACTION_TYPE::ARM;
    else if(ui->audioRadioBtn->isChecked()){
        type = SUBACTION_TYPE::AUDIO;
        if(ui->audioComboBox->currentIndex() >= 8)  //thanks for coming, audio 1~9
            valueList.append((float)(ui->audioComboBox->currentIndex()+2));
        else
            valueList.append((float)(ui->audioComboBox->currentIndex()+1));
    }
    else if(ui->delayRadioBtn->isChecked()){
        type = SUBACTION_TYPE::DELAY;
        valueList.append((float)ui->delaySpinBox->value());
    }
    else if(ui->searchTagRadioBtn->isChecked()){
        type = SUBACTION_TYPE::SEARCHTAG;
        bool ok;
        valueList.append((float)(ui->searchTagComboBox->currentText().toInt(&ok, 10)));
    }
    else if(ui->ledRadioBtn->isChecked()){
        type = SUBACTION_TYPE::LED;
        if(ui->ledComboBox->currentIndex() == 4)    //empty
            valueList.append(99.0);
        else if(ui->ledComboBox->currentText().contains("LED")) //LED 1~6
            valueList.append(ui->ledComboBox->currentIndex() - 5 + 21);
        else
            valueList.append((float)(ui->ledComboBox->currentIndex()+1));
    }
    else if(ui->dynparamRadioBtn->isChecked()){
        type = SUBACTION_TYPE::DYNPARAM;
        valueList.append((float)ui->laserDistanceSpinBox->value());
        valueList.append((float)ui->laserDegreeMinSpinBox->value());
        valueList.append((float)ui->laserDegreeMaxSpinBox->value());
        valueList.append((float)ui->ginflationSpinBox->value());
        valueList.append((float)ui->linflationSpinBox->value());
        valueList.append((float)ui->gFootprintCB->currentIndex());
        valueList.append((float)ui->lFootprintCB->currentIndex());
        valueList.append((float)ui->glWeightScrollBar->value());
        valueList.append((float)ui->globalCombinMethodCB->currentIndex());
        valueList.append((float)ui->localCombinMethodCB->currentIndex());
    }
    else if(ui->imgDetcRadioButton->isChecked()){
        type = SUBACTION_TYPE::IMAGEDETECT;
        int value = 0;
        if(ui->personCB->isChecked())
            value += 1;
        if(ui->chairCB->isChecked())
            value += 2;
        if(ui->tableCB->isChecked())
            value += 4;
        if(ui->otherCB->isChecked())
            value += 8;
        valueList.append((float)value);
    }
    else if(ui->remarksRadioBtn->isChecked()){
        type = SUBACTION_TYPE::REMARKS;
        valueList.append((float)ui->remarkComboBox->currentIndex()+1);
        valueList.append((float)ui->remarkSubComboBox->currentIndex()+1);
    }
    else if(ui->writeExtCommRadioBtn->isChecked()){
        type = SUBACTION_TYPE::WRITEEXTCOMM;
        valueList.append((float)ui->writeExtAddrSpinBox->value());
        valueList.append((float)ui->writeExtValueSpinBox->value());
    }
    else if(ui->requestExtCommRadioBtn->isChecked()){
        type = SUBACTION_TYPE::REQUESTEXTCOMM;
        valueList.append((float)ui->requestExtAddrSpinBox->value());
        valueList.append((float)ui->requestExtValueSpinBox->value());
    }
    else if(ui->comTableDetecRadioBtn->isChecked()){
        type = SUBACTION_TYPE::COMBINETABLEDETECTION;
        valueList.append((float)ui->jumpComboBox->currentIndex());
        if(ui->jumpComboBox->currentIndex() == 0)
            valueList.append((float)ui->jumpSpinBox->value());
        else if(ui->jumpComboBox->currentIndex() == 1){
            valueList.append((float)ui->jumpRemarkComboBox->currentIndex()+1);
            valueList.append((float)ui->jumpRemarkSubComboBox->currentIndex()+1);
        }
    }

    if(isEditAction){
        bool isChange = false;
        if((SUBACTION_TYPE)cmpValueList.first() != type || cmpValueList.count()-1 != valueList.count())
            emit editNaviAction(actionOrder, type, valueList);
        else{
            for(int i=0; i<valueList.count(); i++)
                if((float)cmpValueList.at(i+1) != (float)valueList.at(i)){
                    emit editNaviAction(actionOrder, type, valueList);
                    break;
                }
        }
    }
    else
        emit addNaviAction(type, valueList, true);

    close();
}

/*
*Function Description :
*Parameters :  NONE
*Returns : 		void 
*/
void ActionDialog::on_add2BelowBtn_clicked()
{
    QList<float> valueList;
    if(ui->armRadioBtn->isChecked())
        type = SUBACTION_TYPE::ARM;
    else if(ui->audioRadioBtn->isChecked()){
        type = SUBACTION_TYPE::AUDIO;
        if(ui->audioComboBox->currentIndex() >= 8)  //thanks for coming, audio 1~9
            valueList.append((float)(ui->audioComboBox->currentIndex()+2));
        else
            valueList.append((float)(ui->audioComboBox->currentIndex()+1));
    }
    else if(ui->delayRadioBtn->isChecked()){
        type = SUBACTION_TYPE::DELAY;
        valueList.append((float)ui->delaySpinBox->value());
    }
    else if(ui->searchTagRadioBtn->isChecked()){
        type = SUBACTION_TYPE::SEARCHTAG;
        bool ok;
        valueList.append((float)(ui->searchTagComboBox->currentText().toInt(&ok, 10)));
    }
    else if(ui->ledRadioBtn->isChecked()){
        type = SUBACTION_TYPE::LED;
        if(ui->ledComboBox->currentIndex() == 4)
            valueList.append(99.0);
        else if(ui->ledComboBox->currentText().contains("LED")) //LED 1~6
            valueList.append(ui->ledComboBox->currentIndex() - 5 + 21);
        else
            valueList.append((float)(ui->ledComboBox->currentIndex()+1));
    }
    else if(ui->dynparamRadioBtn->isChecked()){
        type = SUBACTION_TYPE::DYNPARAM;
        valueList.append((float)ui->laserDistanceSpinBox->value());
        valueList.append((float)ui->laserDegreeMinSpinBox->value());
        valueList.append((float)ui->laserDegreeMaxSpinBox->value());
        valueList.append((float)ui->ginflationSpinBox->value());
        valueList.append((float)ui->linflationSpinBox->value());
        valueList.append((float)ui->gFootprintCB->currentIndex());
        valueList.append((float)ui->lFootprintCB->currentIndex());
        valueList.append((float)ui->glWeightScrollBar->value());
        valueList.append((float)ui->globalCombinMethodCB->currentIndex());
        valueList.append((float)ui->localCombinMethodCB->currentIndex());
    }
    else if(ui->imgDetcRadioButton->isChecked()){
        type = SUBACTION_TYPE::IMAGEDETECT;
        int value = 0;
        if(ui->personCB->isChecked())
            value += 1;
        if(ui->chairCB->isChecked())
            value += 2;
        if(ui->tableCB->isChecked())
            value += 4;
        if(ui->otherCB->isChecked())
            value += 8;
        valueList.append((float)value);
    }
    else if(ui->remarksRadioBtn->isChecked()){
        type = SUBACTION_TYPE::REMARKS;
        valueList.append((float)ui->remarkComboBox->currentIndex()+1);
        valueList.append((float)ui->remarkSubComboBox->currentIndex()+1);
    }
    else if(ui->writeExtCommRadioBtn->isChecked()){
        type = SUBACTION_TYPE::WRITEEXTCOMM;
        valueList.append((float)ui->writeExtAddrSpinBox->value());
        valueList.append((float)ui->writeExtValueSpinBox->value());
    }
    else if(ui->requestExtCommRadioBtn->isChecked()){
        type = SUBACTION_TYPE::REQUESTEXTCOMM;
        valueList.append((float)ui->requestExtAddrSpinBox->value());
        valueList.append((float)ui->requestExtValueSpinBox->value());
    }
    else if(ui->comTableDetecRadioBtn->isChecked()){
        type = SUBACTION_TYPE::COMBINETABLEDETECTION;
        valueList.append((float)ui->jumpComboBox->currentIndex());
        if(ui->jumpComboBox->currentIndex() == 0)
            valueList.append((float)ui->jumpSpinBox->value());
        else if(ui->jumpComboBox->currentIndex() == 1){
            valueList.append((float)ui->jumpRemarkComboBox->currentIndex()+1);
            valueList.append((float)ui->jumpRemarkSubComboBox->currentIndex()+1);
        }
    }
    emit addNaviAction(type, valueList, false);
    close();
}

/*
*Function Description :
*Parameters :  NONE
*Returns : 		void 
*/
void ActionDialog::on_allCB_clicked()
{
    bool checked = ui->allCB->isChecked();
    ui->personCB->setChecked(checked);
    ui->chairCB->setChecked(checked);
    ui->tableCB->setChecked(checked);
    ui->otherCB->setChecked(checked);
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_personCB_toggled(bool checked)
{
    if(checked){
        if(ui->personCB->isChecked() && ui->chairCB->isChecked() && ui->tableCB->isChecked() && ui->otherCB->isChecked()){
            ui->allCB->setChecked(true);
        }
    }
    else{
        ui->allCB->setChecked(false);
    }
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_chairCB_toggled(bool checked)
{
    if(checked){
        if(ui->personCB->isChecked() && ui->chairCB->isChecked() && ui->tableCB->isChecked() && ui->otherCB->isChecked()){
            ui->allCB->setChecked(true);
        }
    }
    else{
        ui->allCB->setChecked(false);
    }
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_tableCB_toggled(bool checked)
{
    if(checked){
        if(ui->personCB->isChecked() && ui->chairCB->isChecked() && ui->tableCB->isChecked() && ui->otherCB->isChecked()){
            ui->allCB->setChecked(true);
        }
    }
    else{
        ui->allCB->setChecked(false);
    }
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_otherCB_toggled(bool checked)
{
    if(checked){
        if(ui->personCB->isChecked() && ui->chairCB->isChecked() && ui->tableCB->isChecked() && ui->otherCB->isChecked()){
            ui->allCB->setChecked(true);
        }
    }
    else{
        ui->allCB->setChecked(false);
    }
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_remarkComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->remarksRadioBtn->setChecked(true);
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_remarkSubComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->remarksRadioBtn->setChecked(true);
}

/*
*Function Description :
*Parameters : event Pointer
*Returns : 		void 
*/
void ActionDialog::changeEvent(QEvent *event)
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

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_writeExtCommRadioBtn_toggled(bool checked)
{
    ui->writeExtAddrSpinBox->setEnabled(checked);
    ui->writeExtValueSpinBox->setEnabled(checked);
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_requestExtCommRadioBtn_toggled(bool checked)
{
    ui->requestExtAddrSpinBox->setEnabled(checked);
    ui->requestExtValueSpinBox->setEnabled(checked);
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_agvRadioButton_toggled(bool checked)
{
    if(checked){
        setActionWidget(true);
    }
    else{
        setActionWidget(false);
    }
}

/*
*Function Description :
*Parameters : checked
*Returns : 		void 
*/
void ActionDialog::on_comTableDetecRadioBtn_toggled(bool checked)
{
    ui->jumpSpinBox->setEnabled(checked);
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_jumpComboBox_currentIndexChanged(int index)
{
    if(index == 0){
        ui->jumpSpinBox->setVisible(true);
        ui->jumpRemarkComboBox->setVisible(false);
        ui->jumpRemarkSubComboBox->setVisible(false);
    }
    else if(index == 1){
        ui->jumpSpinBox->setVisible(false);
        ui->jumpRemarkComboBox->setVisible(true);
        ui->jumpRemarkSubComboBox->setVisible(true);
    }
}

/*
*Function Description :
*Parameters : index
*Returns : 		void 
*/
void ActionDialog::on_jumpComboBox_highlighted(int index)
{
    Q_UNUSED(index);
    ui->comTableDetecRadioBtn->setChecked(true);
}
