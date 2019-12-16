#include "failedreasondialog.h"
#include "ui_failedreasondialog.h"

FailedReasonDialog::FailedReasonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FailedReasonDialog)
{
    ui->setupUi(this);
    ui->radioButton_6->setChecked(true);
}

FailedReasonDialog::~FailedReasonDialog()
{
    delete ui;
}

void FailedReasonDialog::on_okBtn_clicked()
{
    FAILED_REASON failedType = FAILED_REASON::AisleObstacle;
    if(ui->radioButton_1->isChecked()){
        failedType = FAILED_REASON::AisleObstacle;
    }
    else if(ui->radioButton_2->isChecked()){
        failedType = FAILED_REASON::FieldChange;
    }
    else if(ui->radioButton_3->isChecked()){
        failedType = FAILED_REASON::GetLost;
    }
    else if(ui->radioButton_4->isChecked()){
        failedType = FAILED_REASON::NotGoing;
    }
    else if(ui->radioButton_5->isChecked()){
        failedType = FAILED_REASON::Other;
    }
    else if(ui->radioButton_6->isChecked()){
        failedType = FAILED_REASON::Testing;
    }

    this->hide();
    emit parseFailedReason(failedType);
    this->close();
}

void FailedReasonDialog::on_cancelBtn_clicked()
{
    this->close();
}

void FailedReasonDialog::changeEvent(QEvent *event)
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
