#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTime>
#include <QCalendarWidget>

namespace Ui {
class RecordDialog;
}

class RecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordDialog(QWidget *parent = nullptr);
    ~RecordDialog();

private slots:
    void on_filterBtn_clicked();
    void on_dateTimeEdit_1_dateTimeChanged(const QDateTime &dateTime);
    void on_dateTimeEdit_2_dateTimeChanged(const QDateTime &dateTime);

private:
    Ui::RecordDialog *ui;
    QFile serviceLogFile;
};

#endif // RECORDDIALOG_H
