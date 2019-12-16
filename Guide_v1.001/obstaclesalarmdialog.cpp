#include "obstaclesalarmdialog.h"
#include "ui_obstaclesalarmdialog.h"

ObstaclesAlarmDialog::ObstaclesAlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObstaclesAlarmDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
}

ObstaclesAlarmDialog::~ObstaclesAlarmDialog()
{
    delete ui;
}

void ObstaclesAlarmDialog::on_closeBtn_clicked()
{
    this->close();
}
