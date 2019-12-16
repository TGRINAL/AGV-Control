#include "editdbdialog.h"
#include "ui_editdbdialog.h"

EditDbDialog::EditDbDialog(bool flag, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->numberLineEdit->setText("");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText("");
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::EditDbDialog(bool flag, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->numberLineEdit->setText("");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::EditDbDialog(bool flag, QString num, QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDbDialog)
{
    ui->setupUi(this);
    ui->numberLineEdit->setInputMask("999");
    ui->idLineEdit->setInputMask(">HHHH");
    ui->numberLineEdit->setText(num);
    ui->idLineEdit->setText(id);
    isAddDb = flag;
    ui->okBtn->setFocus();
}

EditDbDialog::~EditDbDialog()
{
    delete ui;
}
void EditDbDialog::on_okBtn_clicked()
{
    if(ui->idLineEdit->text().isEmpty() || ui->numberLineEdit->text().isEmpty()){
        QMessageBox::critical(this, tr("Error"), "Empty information");
        return;
    }
    if(isAddDb)
        emit addDbFinish(ui->numberLineEdit->text(), ui->idLineEdit->text());
    else
        emit editDbFinish(ui->numberLineEdit->text(), ui->idLineEdit->text());
    close();
}

void EditDbDialog::on_cancelBtn_clicked()
{
    close();
}
