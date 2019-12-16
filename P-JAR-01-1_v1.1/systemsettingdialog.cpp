#include "systemsettingdialog.h"
#include "ui_systemsettingdialog.h"

SystemSettingDialog::SystemSettingDialog(int langIdx, QString volumeStr, QSqlRelationalTableModel *model,
                                         QPointF originPosition, float originAngle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemSettingDialog)
{
    ui->setupUi(this);
    langIndex = langIdx;
    volumeStr.remove("%");
    volumeStr.remove("\n");
    if(volumeStr.isEmpty()){
        ui->volumeScrollBar->setVisible(false);
        ui->volumeLabel->setVisible(false);
        ui->lowLabel->setVisible(false);
        ui->highLabel->setVisible(false);
        ui->testSoundBtn->setVisible(false);
    }
    else{
        bool ok;
        ui->volumeScrollBar->setValue(volumeStr.toInt(&ok, 10));
    }

    tableDbModel = model;

    LightWidget *m_light1 = new LightWidget(Qt::gray);
    LightWidget *m_light2 = new LightWidget(Qt::gray);
    LightWidget *m_light3 = new LightWidget(Qt::gray);
    LightWidget *m_light4 = new LightWidget(Qt::gray);
    LightWidget *m_light5 = new LightWidget(Qt::gray);
    LightWidget *m_light6 = new LightWidget(Qt::gray);

    m_lightList.append(m_light1);
    m_lightList.append(m_light2);
    m_lightList.append(m_light3);
    m_lightList.append(m_light4);
    m_lightList.append(m_light5);
    m_lightList.append(m_light6);

    for(int i=0; i<m_lightList.count(); i++)
        ui->lightGridLayout->addWidget(m_lightList.at(i), 1, i);

    origin_x = originPosition.x();
    origin_y = originPosition.y();
    origin_angle = originAngle;

    ui->originDsb_x->setValue(originPosition.x());
    ui->originDsb_y->setValue(originPosition.y());
    ui->originDsb_angle->setValue(originAngle);

    ui->speechRecogCheckBox->setVisible(false);
}

SystemSettingDialog::~SystemSettingDialog()
{
    delete ui;
}

void SystemSettingDialog::on_shutdownBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::SHUTDOWN);
}

void SystemSettingDialog::on_rebootBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::REBOOT);
}

void SystemSettingDialog::on_resetStatusBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::RESETSTATUS);
}

void SystemSettingDialog::on_cardDbBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::EDITCARDDB);
}

void SystemSettingDialog::on_tableDbBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::EDITTABLEDB);
}

void SystemSettingDialog::changeEvent(QEvent *event)
{
    if(0 != event){
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            break;
        }
        default:
            break;
        }
        QDialog::changeEvent(event);
    }
}

void SystemSettingDialog::on_testSoundBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::TESTSOUND);
    ui->testSoundBtn->setEnabled(false);
    ui->volumeScrollBar->setEnabled(false);
    QTimer::singleShot(5000, this, SLOT(enableVolumeTest()));
}

void SystemSettingDialog::enableVolumeTest()
{
    ui->testSoundBtn->setEnabled(true);
    ui->volumeScrollBar->setEnabled(true);
}

void SystemSettingDialog::on_volumeScrollBar_sliderReleased()
{
    emit setVolume(ui->volumeScrollBar->value());
}

void SystemSettingDialog::on_volumeScrollBar_actionTriggered(int action)
{
    if(action == QAbstractSlider::SliderSingleStepAdd || action == QAbstractSlider::SliderSingleStepSub ||
            action == QAbstractSlider::SliderPageStepAdd || action == QAbstractSlider::SliderPageStepSub){
        emit setVolume(ui->volumeScrollBar->sliderPosition());
    }
}

void SystemSettingDialog::on_performanceModeBtn_clicked()
{
    PerformanceDialog *dlg = new PerformanceDialog(tableDbModel, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, SIGNAL(setKeyPriorityFlag(bool)), this->parentWidget(), SLOT(setKeyPriorityFlag(bool)));
    connect(dlg, SIGNAL(updateVel(VEL_TYPE)), this->parentWidget(), SLOT(updateVel(VEL_TYPE)));
    connect(dlg, SIGNAL(requestPerformance(PERFORMANCE_TYPE, int)), this->parentWidget(), SLOT(requestPerformance(PERFORMANCE_TYPE, int)));
    connect(this->parentWidget(), SIGNAL(updateDeliveryStatus2PerformanceDlg(bool)), dlg, SLOT(updateDeliveryStatus(bool)));
    dlg->setModal(true);
    dlg->setFixedHeight(dlg->size().height());
    dlg->setFixedWidth(dlg->size().width());
    dlg->show();
}

void SystemSettingDialog::on_changePwdBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::CHANGEPWD);
}

void SystemSettingDialog::on_langEnBtn_clicked()
{
    emit setLang(0);
}

void SystemSettingDialog::on_langZhBtn_clicked()
{
    emit setLang(1);
}

void SystemSettingDialog::getUltrasonicValue(int value)
{
    for(int i=0; i<6; i++){
        if((value & (int)pow(2, i)) / (int)pow(2, i))
            m_lightList.at(i)->setColor(Qt::green);
        else
            m_lightList.at(i)->setColor(Qt::red);
    }
}

void SystemSettingDialog::on_originConfirmBtn_clicked()
{
    emit setOrigin(QPointF(ui->originDsb_x->value(), ui->originDsb_y->value()), ui->originDsb_angle->value());

    origin_x = ui->originDsb_x->value();
    origin_y = ui->originDsb_y->value();
    origin_angle = ui->originDsb_angle->value();

    ui->originDsb_x->setStyleSheet("color: rgb(0, 0, 0);");
    ui->originDsb_y->setStyleSheet("color: rgb(0, 0, 0);");
    ui->originDsb_angle->setStyleSheet("color: rgb(0, 0, 0);");
}

void SystemSettingDialog::on_originDsb_x_valueChanged(double arg1)
{
    if(origin_x != (float)arg1)
        ui->originDsb_x->setStyleSheet("color: rgb(255, 0, 0);");
    else
        ui->originDsb_x->setStyleSheet("color: rgb(0, 0, 0);");
}

void SystemSettingDialog::on_originDsb_y_valueChanged(double arg1)
{
    if(origin_y != (float)arg1)
        ui->originDsb_y->setStyleSheet("color: rgb(255, 0, 0);");
    else
        ui->originDsb_y->setStyleSheet("color: rgb(0, 0, 0);");
}

void SystemSettingDialog::on_originDsb_angle_valueChanged(double arg1)
{
    if(origin_angle != (float)arg1)
        ui->originDsb_angle->setStyleSheet("color: rgb(255, 0, 0);");
    else
        ui->originDsb_angle->setStyleSheet("color: rgb(0, 0, 0);");
}

void SystemSettingDialog::on_speechRecogCheckBox_toggled(bool checked)
{
    emit setSpeechRecogSwitch(checked);
}
