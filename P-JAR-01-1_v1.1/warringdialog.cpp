#include "warringdialog.h"
#include "ui_warringdialog.h"

WarringDialog::WarringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarringDialog)
{
    ui->setupUi(this);
}

WarringDialog::~WarringDialog()
{
    delete ui;
}

void WarringDialog::setMessage(QString str)
{
    ui->messageBox->setText(str);
}

void WarringDialog::on_cancelBtn_clicked()
{
    this->reject();
    close();
}

void WarringDialog::on_okBtn_clicked()
{
    this->accept();
    close();
}

void WarringDialog::changeEvent(QEvent *event)
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
