#ifndef DYNPARAMDIALOG_H
#define DYNPARAMDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QStringList>
#include <QObject>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>

#include "dynparamtable.h"

namespace Ui {
class dynparamDialog;
}

class dynparamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit dynparamDialog(QStringList, QWidget *parent = 0);
    ~dynparamDialog();

private slots:
    void on_closeBtn_clicked();
    void on_writeBtn_clicked();

signals:
    void setDynparam(QString);

private:
    Ui::dynparamDialog *ui;
    QStringList oriValueList;
};

#endif // DYNPARAMDIALOG_H
