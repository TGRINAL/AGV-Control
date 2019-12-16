#ifndef SYSTEMSETTINGDIALOG_H
#define SYSTEMSETTINGDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDebug>
#include <QSqlRelationalTableModel>
#include <QPainter>

#include "warringdialog.h"
#include "performancedialog.h"
class MyWidget;
class LightWidget;

enum SYSTEM_CMD {SHUTDOWN = 20, REBOOT, RESETSTATUS, EDITCARDDB, EDITTABLEDB, TESTSOUND, CHANGEPWD};

namespace Ui {
class SystemSettingDialog;
}

class SystemSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSettingDialog(int, QString, QSqlRelationalTableModel *, QPointF, float, QWidget *parent = nullptr);
    ~SystemSettingDialog();

private slots:
    void enableVolumeTest();
    void on_shutdownBtn_clicked();
    void on_rebootBtn_clicked();
    void on_resetStatusBtn_clicked();
    void on_cardDbBtn_clicked();
    void on_tableDbBtn_clicked();
    void on_testSoundBtn_clicked();
    void on_volumeScrollBar_sliderReleased();
    void on_volumeScrollBar_actionTriggered(int action);
    void on_performanceModeBtn_clicked();
    void on_changePwdBtn_clicked();
    void on_langEnBtn_clicked();
    void on_langZhBtn_clicked();
    void on_originConfirmBtn_clicked();
    void on_originDsb_x_valueChanged(double arg1);
    void on_originDsb_y_valueChanged(double arg1);
    void on_originDsb_angle_valueChanged(double arg1);
    void on_speechRecogCheckBox_toggled(bool checked);

public slots:
    void getUltrasonicValue(int);

signals:
    void systemCommand(SYSTEM_CMD);
    void setVolume(int);
    void setLang(int);
    void setOrigin(QPointF, float);
    void setSpeechRecogSwitch(bool);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::SystemSettingDialog *ui;
    QSqlRelationalTableModel *tableDbModel;
    int langIndex;
    QList<LightWidget*> m_lightList;
    float origin_x, origin_y, origin_angle;
};

class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = 0)
        : QWidget(parent), m_color(color), m_on(true) {}

    bool isOn() const
    { return m_on; }
    void setOn(bool on)
    {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }
    void setColor(QColor color)
    {
        m_color = color;
        update();
    }

public slots:
    void turnOff() { setOn(false); }
    void turnOn() { setOn(true); }

protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
    {
        if (!m_on)
            return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(m_color);
        int size = 0;
        size = 20;
        painter.drawEllipse((width()-size)/2, (height()-size)/2, size, size);
    }

private:
    QColor m_color;
    bool m_on;
};

#endif // SYSTEMSETTINGDIALOG_H
