#ifndef OBSTACLESALARMDIALOG_H
#define OBSTACLESALARMDIALOG_H

#include <QDialog>

namespace Ui {
class ObstaclesAlarmDialog;
}

class ObstaclesAlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObstaclesAlarmDialog(QWidget *parent = nullptr);
    ~ObstaclesAlarmDialog();

private slots:
    void on_closeBtn_clicked();

private:
    Ui::ObstaclesAlarmDialog *ui;
};

#endif // OBSTACLESALARMDIALOG_H
