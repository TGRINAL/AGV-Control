#include "performancedialog.h"
#include "ui_performancedialog.h"

PerformanceDialog::PerformanceDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformanceDialog),
    isDeliveryFlag(false)
{
    ui->setupUi(this);

    armTimer = new QTimer;
    connect(armTimer, &QTimer::timeout, this, &PerformanceDialog::handleArmTimeout);

    audioTimer = new QTimer;
    connect(audioTimer, &QTimer::timeout, this, &PerformanceDialog::handleAudioTimeout);

    ledTimer = new QTimer;
    connect(ledTimer, &QTimer::timeout, this, &PerformanceDialog::handleLedTimeout);

    ui->tableCb->clear();
    for(int i=0; i<model->rowCount(); ++i)
        ui->tableCb->addItem(tr("Table ") + model->index(i, 1).data().toString());
}

PerformanceDialog::~PerformanceDialog()
{
    emit setKeyPriorityFlag(true);
    delete armTimer;
    delete audioTimer;
    delete ledTimer;
    delete ui;
}

void PerformanceDialog::on_forwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Forward);
}

void PerformanceDialog::on_backwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Backward);
}

void PerformanceDialog::on_leftBtn_pressed()
{
    emit updateVel(VEL_TYPE::Left);
}

void PerformanceDialog::on_rightBtn_pressed()
{
    emit updateVel(VEL_TYPE::Right);
}

void PerformanceDialog::on_stopBtn_clicked()
{
    emit updateVel(VEL_TYPE::Stop);
}

void PerformanceDialog::on_safeStopBtn_clicked()
{
    if(ui->safeStopBtn->text() == tr("Safe Stop")){
        ui->safeStopBtn->setText(tr("No safe stop"));
        ui->safeStopBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
        emit setKeyPriorityFlag(false);
    }
    else if(ui->safeStopBtn->text() == tr("No safe stop")){
        ui->safeStopBtn->setText(tr("Safe Stop"));
        ui->safeStopBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        emit setKeyPriorityFlag(true);
    }
}

void PerformanceDialog::changeEvent(QEvent *event)
{
    if(0 != event) {
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

void PerformanceDialog::on_armBtn_clicked()
{
    emit requestPerformance(PERFORMANCE_TYPE::ARM_TYPE, ui->armCb->currentIndex()+1);
}

void PerformanceDialog::on_audioBtn_clicked()
{
    if(ui->audioCb->currentIndex() >= 8)
        emit requestPerformance(PERFORMANCE_TYPE::AUDIO_TYPE, ui->audioCb->currentIndex()+2);
    else
        emit requestPerformance(PERFORMANCE_TYPE::AUDIO_TYPE, ui->audioCb->currentIndex()+1);
}

void PerformanceDialog::on_ledBtn_clicked()
{
    if(ui->ledCb->currentIndex() == 4)
        emit requestPerformance(PERFORMANCE_TYPE::LED_TYPE, 99);
    else
        emit requestPerformance(PERFORMANCE_TYPE::LED_TYPE, ui->ledCb->currentIndex()+1);
}

void PerformanceDialog::on_abortBtn_clicked()
{
    ui->tableCb->setEnabled(true);
    ui->deliveryBtn->setEnabled(true);
    ui->goOriginBtn->setEnabled(true);
    ui->resetOriginBtn->setEnabled(true);
    emit requestPerformance(PERFORMANCE_TYPE::ABORT_TYPE, 0);
}

void PerformanceDialog::on_goOriginBtn_clicked()
{
    isDeliveryFlag = true;
    emit requestPerformance(PERFORMANCE_TYPE::GOORIGIN_TYPE, 0);
    ui->tableCb->setEnabled(!isDeliveryFlag);
    ui->deliveryBtn->setEnabled(!isDeliveryFlag);
    ui->goOriginBtn->setEnabled(!isDeliveryFlag);
    ui->resetOriginBtn->setEnabled(!isDeliveryFlag);
}

void PerformanceDialog::on_resetOriginBtn_clicked()
{
    emit requestPerformance(PERFORMANCE_TYPE::RESETORIGIN_TYPE, 0);
}

void PerformanceDialog::on_closeBtn_clicked()
{
    this->close();
}

void PerformanceDialog::on_deliveryBtn_clicked()
{
    isDeliveryFlag = true;
    emit requestPerformance(PERFORMANCE_TYPE::DELIVERY_TYPE, ui->tableCb->currentIndex());
    ui->tableCb->setEnabled(!isDeliveryFlag);
    ui->deliveryBtn->setEnabled(!isDeliveryFlag);
    ui->goOriginBtn->setEnabled(!isDeliveryFlag);
    ui->resetOriginBtn->setEnabled(!isDeliveryFlag);
}

void PerformanceDialog::updateDeliveryStatus(bool flag)
{
    isDeliveryFlag = flag;
    ui->tableCb->setEnabled(!flag);
    ui->deliveryBtn->setEnabled(!flag);
    ui->goOriginBtn->setEnabled(!flag);
    ui->resetOriginBtn->setEnabled(!flag);
}

void PerformanceDialog::handleArmTimeout()
{
    armTimer->stop();
    ui->armCb->setEnabled(true);
    ui->armBtn->setEnabled(true);
}

void PerformanceDialog::handleAudioTimeout()
{
    audioTimer->stop();
    ui->audioCb->setEnabled(true);
    ui->audioBtn->setEnabled(true);
}

void PerformanceDialog::handleLedTimeout()
{
    ledTimer->stop();
    ui->ledCb->setEnabled(true);
    ui->ledBtn->setEnabled(true);
}
