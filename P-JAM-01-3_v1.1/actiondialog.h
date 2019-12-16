#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QLineEdit>

#define SERVICE_ROBOT

enum SUBACTION_TYPE {ARM = 200, DELAY, AUDIO, SEARCHTAG, LED, DYNPARAM, IMAGEDETECT, REMARKS, WRITEEXTCOMM, REQUESTEXTCOMM, COMBINETABLEDETECTION};

namespace Ui {
class ActionDialog;
}

class ActionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionDialog(bool, bool, QWidget *parent = 0);
    ActionDialog(bool, int, SUBACTION_TYPE, int, QWidget *parent = 0);
    ActionDialog(bool, int, SUBACTION_TYPE, QList<float>, QWidget *parent = 0);
    ~ActionDialog();

private slots:
    void on_cancelBtn_clicked();
    void on_delayRadioBtn_toggled(bool checked);
    void on_audioComboBox_highlighted(int index);
    void on_searchTagComboBox_highlighted(int index);
    void on_ledComboBox_highlighted(int index);
    void on_dynparamRadioBtn_toggled(bool checked);
    void on_glWeightScrollBar_valueChanged(int value);
    void on_glSpinBox_valueChanged(int arg1);
    void on_add2EndBtn_clicked();
    void on_add2BelowBtn_clicked();
    void on_personCB_toggled(bool checked);
    void on_chairCB_toggled(bool checked);
    void on_tableCB_toggled(bool checked);
    void on_otherCB_toggled(bool checked);
    void on_allCB_clicked();
    void on_imgDetcRadioButton_toggled(bool checked);
    void on_remarkComboBox_highlighted(int index);
    void on_remarkSubComboBox_highlighted(int index);
    void on_writeExtCommRadioBtn_toggled(bool checked);
    void on_requestExtCommRadioBtn_toggled(bool checked);
    void on_agvRadioButton_toggled(bool checked);
    void on_comTableDetecRadioBtn_toggled(bool checked);
    void on_jumpComboBox_currentIndexChanged(int index);

    void on_jumpComboBox_highlighted(int index);

signals:
    void addNaviAction(SUBACTION_TYPE, QList<float>, bool);
    void editNaviAction(int, SUBACTION_TYPE, QList<float>);
    void setAgvOnlyFlag(bool);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::ActionDialog *ui;
    SUBACTION_TYPE type;
    int isEditAction;
    int actionOrder;
    bool allCheckFlag;
    QList<float> cmpValueList;

    void setSomeWidgetInvisable();
    void setActionWidget(bool);
};

#endif // ACTIONDIALOG_H
