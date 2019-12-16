#ifndef FAILEDREASONDIALOG_H
#define FAILEDREASONDIALOG_H

#include <QDialog>

enum FAILED_REASON {AisleObstacle = 11, FieldChange, GetLost, NotGoing, Other, Testing};

namespace Ui {
class FailedReasonDialog;
}

class FailedReasonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FailedReasonDialog(QWidget *parent = nullptr);
    ~FailedReasonDialog();

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

signals:
    void parseFailedReason(FAILED_REASON);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::FailedReasonDialog *ui;
};

#endif // FAILEDREASONDIALOG_H
