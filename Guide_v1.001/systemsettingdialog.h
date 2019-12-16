#ifndef SYSTEMSETTINGDIALOG_H
#define SYSTEMSETTINGDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDebug>
#include <QSqlRelationalTableModel>
#include <QPainter>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QMediaPlayer>

#include "warringdialog.h"
#include "recorddialog.h"
#include "soundmodifydialog.h"

//class MyWidget;
class LightWidget;

enum SYSTEM_CMD {SHUTDOWN = 20, REBOOT, RESETSTATUS, TESTSOUND, CHANGEPWD};

namespace Ui {
class SystemSettingDialog;
}

struct TcpSocketInfo{
    QString ip;
    QString port;
    QString username;
    QString password;
};

struct Origin {
    QPointF position;
    float angle;
    QString name;
};

struct ScriptButton {
    QString buttonName;
    QString scriptName;
    QPointF originPosition;
    float originAngle;
};

class SystemSettingDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SystemSettingDialog(int, QString, QList<ScriptButton *>, TcpSocketInfo *, QString, QWidget *parent = nullptr);
    ~SystemSettingDialog();

private slots:
    void enableVolumeTest();
    void on_shutdownBtn_clicked();
    void on_rebootBtn_clicked();
    void on_resetStatusBtn_clicked();
    void on_testSoundBtn_clicked();
    void on_volumeScrollBar_sliderReleased();
    void on_volumeScrollBar_actionTriggered(int action);
    void on_langEnBtn_clicked();
    void on_langZhBtn_clicked();
    void on_originDsb_x_valueChanged(double arg1);
    void on_originDsb_y_valueChanged(double arg1);
    void on_originDsb_angle_valueChanged(double arg1);
    void on_changePwdBtn_clicked();
    void on_originCB_currentIndexChanged(int index);
    void on_bgmBtn_clicked();
    void on_serviceRecordBtn_clicked();
    void on_closeBtn_clicked();
    void on_soundModifyBtn_clicked();

public slots:
    void getUltrasonicValue(int);

signals:
    void systemCommand(SYSTEM_CMD);
    void setVolume(int);
    void setLang(int);
    void setTcpSocket(QString, QString);
    void askScriptList();
    void setCurrentScript(QString);
    void setStopTime(double);

protected:
    void changeEvent(QEvent *event);

private:
    Ui::SystemSettingDialog *ui;
    int langIndex;
    QList<LightWidget*> m_lightList;
    QString m_currentScript;
    TcpSocketInfo *m_socket;
    int m_currentStopMin, m_currentStopSec;
    QList<ScriptButton *> m_buttonList;
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
        /*if(width() >= height())
            size = height();
        else
            size = width();*/
        size = 20;
        painter.drawEllipse((width()-size)/2, (height()-size)/2, size, size);
        //painter.drawEllipse(0, 0, width(), height());
    }

private:
    QColor m_color;
    bool m_on;
};

#endif // SYSTEMSETTINGDIALOG_H
