#ifndef EDITDBDIALOG_H
#define EDITDBDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class EditDbDialog;
}

class EditDbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDbDialog(bool flag, QWidget *parent = 0);
    EditDbDialog(bool flag, QString id, QWidget *parent = 0);
    EditDbDialog(bool flag, QString num, QString id, QWidget *parent = 0);
    ~EditDbDialog();

signals:
    void addDbFinish(QString, QString);
    void editDbFinish(QString, QString);

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

private:
    Ui::EditDbDialog *ui;
    QString number;
    QString identifier;
    bool isAddDb;
};

#endif // EDITDBDIALOG_H
