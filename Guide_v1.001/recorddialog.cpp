#include "recorddialog.h"
#include "ui_recorddialog.h"

RecordDialog::RecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordDialog)
{
    ui->setupUi(this);

    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());

    QDate date = QDateTime::currentDateTime().date();
    date.setDate(date.year(), date.month(), 1);
    ui->dateTimeEdit_1->setDate(date);
    ui->dateTimeEdit_1->setTime(QTime(0,0));

    on_filterBtn_clicked();
}

RecordDialog::~RecordDialog()
{
    if(serviceLogFile.isOpen())
        serviceLogFile.close();
    delete ui;
}

void RecordDialog::on_filterBtn_clicked()
{
    ui->tableWidget->clearContents();
    if(!serviceLogFile.isOpen()){
        serviceLogFile.setFileName("serviceLog.txt");
        serviceLogFile.open(QFile::ReadOnly | QFile::Text);

        QTextStream inStream(&serviceLogFile);
        int row = 0;
        //ui->tableWidget->setRowCount(5);
        while (!inStream.atEnd())
        {
            QString line = inStream.readLine();
            QStringList currentLine = line.split("\t");
            //qDebug() << line << currentLine.count();
            if(currentLine.count() == 5){
                QDateTime dateTime;
                dateTime.setDate(QDate::fromString(currentLine[0], "yyyy/MM/dd"));
                dateTime.setTime(QTime::fromString(currentLine[1], "hh:mm:ss"));

                if(dateTime > ui->dateTimeEdit_1->dateTime() && dateTime < ui->dateTimeEdit_2->dateTime()){
                    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(currentLine[0]));
                    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(currentLine[1]));
                    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(currentLine[3]));
                    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(currentLine[2]));
                    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(currentLine[4]));
                    QTime diffTime = QTime::fromMSecsSinceStartOfDay(QTime::fromString(currentLine[1], "hh:mm:ss").msecsTo(QTime::fromString(currentLine[3], "hh:mm:ss")));
                    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(diffTime.toString("hh:mm:ss")));
                    row++;
                }
            }
        }
        ui->tableWidget->setRowCount(row);
        serviceLogFile.close();
    }

    for(int i=0; i<6; i++)
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

    ui->tableWidget->repaint();
    ui->filterBtn->setFocus();
}

void RecordDialog::on_dateTimeEdit_1_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->dateTimeEdit_1->dateTime() > ui->dateTimeEdit_2->dateTime())
        ui->dateTimeEdit_1->setDateTime(ui->dateTimeEdit_2->dateTime());
}

void RecordDialog::on_dateTimeEdit_2_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->dateTimeEdit_2->dateTime() < ui->dateTimeEdit_1->dateTime())
        ui->dateTimeEdit_2->setDateTime(ui->dateTimeEdit_1->dateTime());
}
