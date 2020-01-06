/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            mywidget.h
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/


#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QHostAddress>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QScrollBar>
#include <QPointF>
#include <QPoint>
#include <QPainter>
#include <QtMath>
#include <QQuaternion>
#include <QMouseEvent>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPointer>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QWheelEvent>
#include <QImageReader>
#include <QMatrix>
#include <QFileDialog>
#include <QUndoStack>
#include <QUndoCommand>
#include <QAction>
#include <QInputDialog>
#include <QProgressDialog>
#include <QMenu>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTableWidgetItem>
#include <QPalette>
#include <QThread>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTranslator>
#include <QTextCodec>
#include <QPainter>
#include <QProcess>

#include "dbwidgt.h"
#include "failedreasondialog.h"
#include "joystickdialog.h"
#include "warringdialog.h"
#include "listdialog.h"
#include "systemsettingdialog.h"
#include "changepwddialog.h"
#include "batteryitem.h"

#define VOLTAGE_MAX 28.5
#define VOLTAGE_MIN 23.5

//enum VEL_TYPE {Forward, Backward, Right, Left, Stop};
enum MODE_TYPE {SLAM = 50, NAVI, TEACHPOINT, EDITMAP, XROCKER, DELIVERY, RESETNODE, RESTARTAGV, SHUTDOWNAGV};
enum ACTION_TYPE {NAVIGOAL = 60, ARMACTION, AUDIOPLAY, DELAYACTION, SEARCH_TAG, CORRECT_POINT, DELIVERY_POINT, LEDACTION, DYNPARAMACTION, IMGDETECTACTION, NOACTION};
enum NAVIMODE_TYPE {Normal = 100, SetInitPose, AddGoal, EditGoal, StartAction};
enum DELIVERYMODE_TYPE{DeliveryStop, DeliveryPause, DeliveryStart};
enum NAVISTATE {Null = 110, Pending, Active, Preempted, Succeeded, Aborted
                , Rejected, Preempting, Recalling, Recalled, Lost};

namespace Ui {
class MyWidget;
}

/*
* Class Description
*/
class MyWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    struct deliveryItem {
        QString tableNum;
        QString cardNum;
    };

    struct origin {
        QPointF position;
        float angle;
    } originPoint{QPointF(0.0, 0.0), 0.0};

    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

    MODE_TYPE mode;
    NAVIMODE_TYPE naviMode;
    NAVISTATE naviState;
    VEL_TYPE velType;
    DELIVERYMODE_TYPE deliveryMode;

    QString password() const
    {
        return m_password;
    }

private slots:
    void tcpConnectStateChange();
    void readData();
    void on_connectBtn_clicked();
    void on_serialOpenBtn_clicked();
    void on_deliveryPauseBtn_clicked();
    void on_deliveryStopBtn_clicked();
    void on_deliveryTableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_deliveryJoystickBtn_clicked();
    void on_refreshSerialBn_clicked();
    void on_deliveryUpdatePoseBtn_clicked();
    void on_systemBtn_clicked();

public slots:
    void updateVel(VEL_TYPE);
    void loadScript(QString);
    void joystickDestroyed();
    void readSerialData();
    void deliveryAction();
    void deliveryPerformance(int);
    void clickAddDeskDb();
    void clickAddCardDb();
    void goOrigin();
    void poseUpdate();
    void abortMovebase();
    void setTeleopMode(TELEOP_TYPE);
    void setKeyPriorityFlag(bool);
    void deskAskListScript();
    void parseFailedReason(FAILED_REASON);
    void setPassword(QString password){m_password = password;}
    void on_deliveryResetOriginBtn_clicked();
    void joystick4Administrator();
    void setVolume(int);
    void systemCommand(SYSTEM_CMD);
    void setLang(int);
    void requestPerformance(PERFORMANCE_TYPE, int);
    void setAudioOff();
    void setOrigin(QPointF, float);
    void setSpeechRecogSwitch(bool);

private:
    Ui::MyWidget *ui;
    QTranslator *trsChinese;
    QTcpSocket *tcpSocket;
    int port;
    QHostAddress *serverIP;
    QByteArray allData;
    float velPercentage;
    QTimer *velCmdTimer;
    QTimer *enableUiTimer;
    QSerialPort *serial;
    DbWidgt *deskDb;
    DbWidgt *cardDb;
    QList<deliveryItem *> deliveryList;
    QString deskId, cardId;
    TELEOP_TYPE teleopMode;
    QString currentDeliveryScriptName;
    bool keyPriorityFlag;
    QFile connectionLogFile, deliveryResultLogFile;
    QTextStream deliveryLogStream;
    QDateTime beginConnectedDateTime, beginDeliveryDateTime;
    JoystickDialog *emergencyDlg = nullptr;
    ListDialog *listDlg;
    QMediaPlaylist *playlist;
    QMediaPlayer *mediaPlayer;
    int poseUpdateNum;
    QString langFileName;
    int langIdx;    //0:EN, 1:ZH
    SystemSettingDialog *systemSettingDlg;
    QMessageBox *dbAddMessageBox;
    WarringDialog *checkVoiceDeliverydlg;
    QString versionNum;
    BatteryItem *batItem;
    QString currentDeliveryCT;

    void parseMovebaseStatusJsonData(QByteArray data);
    void parseFeedbackMsg(QByteArray data);
    void parseSensorButtonCmdVel(QByteArray data);
    void parseLongTimeStopAlarm(QByteArray data);
    void parseListScriptServiceResponseJsonData(QByteArray data);
    void parseGetVolumeJsonData(QByteArray data);
    void parseRealVolState(QByteArray data);
    void parseUltraSensorStatus(QByteArray data);

    void writeSettings();
    void readSettings();
    void handleEnableUiTimeout();
    void handleVelCmdTimeout();


    void initDelivery();
    void openSerialPort();
    void closeSerialPort();
    void setDeliveryTable();
    void enableUi(bool);

    //Advertise
    void onAdvertiseCmdVel();
    void onAdvertiseRobotModeFlag();
    void onAdvertiseActionScript();
    void onAdvertiseAskScript();
    void onAdvertiseActionCommand();
    void onAdvertiseKeyPriority();

    //Unadvertise
    void onUnadvertiseRobotModeFlag();

    //publish
    void onPublishVelCmd();
    void onPublishInitPose(bool, QPointF, QQuaternion);
    void onPublishRobotModeFlag(MODE_TYPE);
    void onPublishActionCommand(int, QString, int);
    void onPublishKeyPriority(bool);
    void onPublishAudio(int);
    void onPublishPeripheral(int, int); //arm, led
    void onPublishMoveBaseCancel();
    void onPublishMoveBaseGoalForOrigin();
    void onPublishSpeechRecognitionSwitch(bool);

    //subscribe
    void onSubscribeMovebaseStatus();
    void onSubscribeFeedbackMsg();
    void onSubscribeSensorButtonCmdVel();
    void onSubscribeLongTimeStopAlarm();
    void onSubscribeRealVolState();
    void onSubscribeUltraSensorStatus();

    //unsubscribe
    void onUnsubscribeMovebaseStatus();
    void onUnsubscribeFeedbackMsg();
    void onUnsubscribeSensorButtonCmdVel();
    void onUnsubscribeLongTimeStopAlarm();
    void onUnsubscribeRealVolState();
    void onUnsubscribeUltraSensorStatus();

    //service
    void onCallServiceListScript();
    void onCallServiceDeleteScript(QString);
    void onCallServiceUpdatePose();
    void onCallServiceLoadMap(QString);
    void onCallServiceGetVolume();
    void onCallServiceSetVolume(int);

    QString m_password;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);

signals:
    void updateAmclPose2TeachDlg(QPointF, QQuaternion);
    void replyTerminalPoints(QPointF, QPointF);
    void addEmptyCardDb(QString);
    void addEmptyDeskDb(QString);
    void deskGetScriptName(QString);
    void updateDeliveryStatus2PerformanceDlg(bool);
    void updateUltrasonicValue(int);
};

#endif // MYWIDGET_H
