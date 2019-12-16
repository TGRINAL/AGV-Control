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
#include <QTableWidgetItem>
#include <QPalette>
#include <QThread>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTranslator>
#include <QTextCodec>
#include <QPainter>
#include <QProcess>
#include <QSettings>
#include <QtMath>
#include <QTextStream>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QUrl>
#include <QTextCodec>

#include "failedreasondialog.h"
#include "joystickdialog.h"
#include "warringdialog.h"
#include "listdialog.h"
#include "systemsettingdialog.h"
#include "changepwddialog.h"
#include "batteryitem.h"
#include "resetorigindialog.h"
#include "obstaclesalarmdialog.h"

#define DEGREETORADIAN(degree) (degree/180.0*M_PI)
#define RADIANTODEGREE(radian) (radian/M_PI*180.0)
#define VOLTAGE_MAX 28.5
#define VOLTAGE_MIN 23.5

//enum VEL_TYPE {Forward, Backward, Right, Left, Stop};
enum MODE_TYPE {SLAM = 50, NAVI, TEACHPOINT, EDITMAP, XROCKER, DELIVERY, RESETNODE, RESTARTAGV, SHUTDOWNAGV};

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:

    /*struct Origin {
        QPointF position;
        float angle;
    };//originPoint{QPointF(0.0, 0.0), 0.0};
*/
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

    MODE_TYPE mode;
    VEL_TYPE velType;

    QString password() const
    {
        return m_password;
    }

private slots:
    void tcpConnectStateChange();
    void readData();
    void on_connectBtn_clicked();
    void on_joystickBtn_clicked();
    void on_resetOriginBtn_clicked();
    void on_managementBtn_clicked();
    void on_goBtn_clicked();
    void on_abortBtn_clicked();
    void on_systemShutdownBtn_clicked();

public slots:
    void updateVel(VEL_TYPE);
    void loadScript(QString);
    void joystickDestroyed();
    void goOrigin();
    void poseUpdate();
    void poseUpdateOnce();
    void abortMovebase();
    void setTeleopMode(TELEOP_TYPE);
    void setKeyPriorityFlag(bool);
    void setPassword(QString password){m_password = password;}
    void resetOrigin(int);
    void joystick4Administrator();
    void setVolume(int);
    void systemCommand(SYSTEM_CMD);
    void setLang(int);
    void setTcpSocket(QString, QString);
    void checkPassword(QString);
    void askScriptList();
    void setCurrentScript(QString);
    void alarmDlagDestroyed();
    void startFromWhere(int);
    void startScript(int);
    void showAction(int, int);

private:
    Ui::MyWidget *ui;
    QTranslator *trsChinese;
    QTcpSocket *tcpSocket;
    QByteArray allData;
    TcpSocketInfo *m_tcpSocketInfo;
    float velPercentage;
    QTimer *velCmdTimer;
    QTimer *enableUiTimer;
    QTimer *progressTimer;
    QTimer *audioLoopTimer;
    TELEOP_TYPE teleopMode;
    bool keyPriorityFlag;
    QFile connectionLogFile, serviceLogFile;
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
    QString versionNum;
    BatteryItem *batItem;
    QList<Origin *> originList;
    QString currentScript;
    QString currentMap;
    QJsonArray scriptActionJsonAry;
    ObstaclesAlarmDialog *obstaclesAlarmDlg = nullptr;
    QProgressDialog *progressDlg;
    int progressValue;
    int audioLoopIndex;
    QList<ScriptButton *> scriptButtonList;

    void parseFeedbackMsg(QByteArray data);
    void parseSensorButtonCmdVel(QByteArray data);
    void parseLongTimeStopAlarm(QByteArray data);
    void parseListScriptServiceResponseJsonData(QByteArray data);
    void parseGetVolumeJsonData(QByteArray data);
    void parseRealVolState(QByteArray data);
    void parseUltraSensorStatus(QByteArray data);
    void parseReplyScript(QByteArray data);

    void writeSettings();
    void readSettings();
    void handleEnableUiTimeout();
    void handleVelCmdTimeout();
    void handleProgressTimeout();
    void handleAudioLoopTimeout();
    void correctRobotDateTime();
    void setProgressDialog();
    void writeScriptSettingToTxt();
    void readScriptSettingFromTxt();
    float checkAudioSec(int);

    void enableUi(bool);

    //Advertise
    void onAdvertiseRobotModeFlag();
    void onAdvertiseCmdVel();
    void onAdvertiseActionCommand();
    void onAdvertiseActionScript();
    void onAdvertiseAskScript();
    void onAdvertiseKeyPriority();
    void onAdvertiseInitPose();
    void onAdvertiseMoveBaseCancel();
    void onAdvertiseBaseGoalForOrigin();
    void onAdvertiseRobotSoundFlag();
    void onAdvertiseWriteExternalComm();

    //publish
    void onPublishRobotModeFlag(MODE_TYPE);
    void onPublishCmdVel();
    void onPublishActionCommand(int, QString, int);
    bool onPublishActionScript();
    void onPublishAskScript(QString);
    void onPublishKeyPriority(bool);
    void onPublishInitPose(bool, QPointF, QQuaternion);
    void onPublishMoveBaseCancel();
    void onPublishMoveBaseGoalForOrigin();
    void onPublishRobotSoundFlag(int);
    void onPublishWriteExternalComm(int);

    //subscribe
    void onSubscribeFeedbackMsg();
    void onSubscribeSensorButtonCmdVel();
    void onSubscribeLongTimeStopAlarm();
    void onSubscribeRealVolState();
    void onSubscribeReplyScript();
    void onSubscribeUltraSensorStatus();

    //unsubscribe
    void onUnsubscribeFeedbackMsg();
    void onUnsubscribeSensorButtonCmdVel();
    void onUnsubscribeLongTimeStopAlarm();
    void onUnsubscribeRealVolState();
    void onUnsubscribeReplyScript();
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
    void sendScriptList(QStringList);
    void sendStopTime(double);
    void sendVel(float, float);
};

#endif // MYWIDGET_H
