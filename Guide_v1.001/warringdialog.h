#ifndef WARRINGDIALOG_H
#define WARRINGDIALOG_H

#include <QDialog>

namespace Ui {
class WarringDialog;
}

class WarringDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WarringDialog(QWidget *parent = nullptr);
    ~WarringDialog();
    void setMessage(QString);
    void setCancelBtn(bool);

private slots:
    void on_cancelBtn_clicked();
    void on_okBtn_clicked();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::WarringDialog *ui;
};

#endif // WARRINGDIALOG_H
