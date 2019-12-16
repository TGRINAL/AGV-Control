#ifndef CHANGEPWDDIALOG_H
#define CHANGEPWDDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class ChangePwdDialog;
}

class ChangePwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwdDialog(QWidget *parent = nullptr);
    ~ChangePwdDialog();
    static QString getOnePassword(QWidget *parent = nullptr, bool *ok = nullptr);
    void setOnePasswordMode();

private slots:
    void on_cancelBtn_clicked();
    void on_confirmBtn_clicked();

    void on_showPwCheckBox_toggled(bool checked);

signals:
    void setNewPassword(QString);
    void checkPassword(QString);

private:
    Ui::ChangePwdDialog *ui;
    bool isOnePasswordMode;
};

#endif // CHANGEPWDDIALOG_H
