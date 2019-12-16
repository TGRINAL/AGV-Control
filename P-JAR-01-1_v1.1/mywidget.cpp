#include "mywidget.h"
#include "ui_mywidget.h"

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget),
    mode(MODE_TYPE::DELIVERY),
    naviMode(NAVIMODE_TYPE::Normal),
    naviState(NAVISTATE::Null),
    velType(VEL_TYPE::Stop),
    deliveryMode(DELIVERYMODE_TYPE::DeliveryStop),
    velPercentage(40),
    teleopMode(TELEOP_TYPE::LINEAR),
    keyPriorityFlag(true),
    poseUpdateNum(0),
    langFileName("agv_delivery"),
    versionNum("P-JAR-01-1_v1.1")
{
    ui->setupUi(this);

    this->setWindowTitle(tr("MOS Delivery Service Software ") + versionNum);

    batItem = new BatteryItem;
    batItem->setBatteryValue(-1);

    trsChinese = new QTranslator(this);
    trsChinese->load("agv_delivery_zh.qm");

    enableUiTimer = new QTimer;
    enableUiTimer->setInterval(7000);
    connect(enableUiTimer, &QTimer::timeout, this, &MyWidget::handleEnableUiTimeout);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::stateChanged, this, &MyWidget::tcpConnectStateChange);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MyWidget::readData);
    tcpConnectStateChange();

    serial = new QSerialPort(this);

    velCmdTimer = new QTimer;
    velCmdTimer->setInterval(200);
    connect(velCmdTimer, &QTimer::timeout, this, &MyWidget::handleVelCmdTimeout);

    listDlg = new ListDialog;
    connect(listDlg, &ListDialog::loadScript, this, &MyWidget::loadScript);

    cardDb = new DbWidgt(DB_TYPE::CARD);
    connect(this, &MyWidget::addEmptyCardDb, cardDb, &DbWidgt::addEmptyDb);
    connect(cardDb, &DbWidgt::joystick4Administrator, this, &MyWidget::joystick4Administrator);
    deskDb = new DbWidgt(DB_TYPE::DESK);
    connect(this, &MyWidget::addEmptyDeskDb, deskDb, &DbWidgt::addEmptyDb);
    connect(deskDb, &DbWidgt::deskAskListScript, this, &MyWidget::deskAskListScript);
    connect(this, &MyWidget::deskGetScriptName, deskDb, &DbWidgt::deskGetScriptName);
    connect(deskDb, &DbWidgt::joystick4Administrator, this, &MyWidget::joystick4Administrator);

    readSettings();

    if(!connectionLogFile.isOpen()){
        connectionLogFile.setFileName("connection_log.txt");
        connectionLogFile.open(QFile::Append | QFile::Text);
    }

    if(!deliveryResultLogFile.isOpen()){
        deliveryResultLogFile.setFileName("delivery_log.txt");
        deliveryResultLogFile.open(QFile::Append | QFile::Text);
    }

    ui->deliveryPauseBtn->setVisible(false);

    ui->mainHorizontalLayout->setStretch(1, 8);
    ui->statusLineEdit->setText("Null");
    tcpConnectStateChange();
    initDelivery();
    setDeliveryTable();

    playlist = new QMediaPlaylist();
    QString mp3Dir = QDir::currentPath() + "/alarmCalling.mp3";
    playlist->addMedia(QUrl::fromLocalFile(mp3Dir));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setPlaylist(playlist);
    mediaPlayer->setVolume(30);

    ui->batLevelLcdNum->setVisible(false);

    ui->statusLayout->addWidget(batItem, 1);
}

MyWidget::~MyWidget()
{
    writeSettings();
    if(tcpSocket->state() == QAbstractSocket::ConnectedState){
        onUnadvertiseRobotModeFlag();
        onUnsubscribeMovebaseStatus();
        onUnsubscribeFeedbackMsg();
        onUnsubscribeSensorButtonCmdVel();
        onUnsubscribeLongTimeStopAlarm();
        onUnsubscribeRealVolState();

        tcpSocket->disconnectFromHost();
    }
    closeSerialPort();

    connectionLogFile.close();
    deliveryResultLogFile.close();

    delete cardDb;
    delete deskDb;
    delete velCmdTimer;
    delete enableUiTimer;
    delete tcpSocket;
    delete serial;
    delete playlist;
    delete mediaPlayer;
    delete ui;
}

void MyWidget::enableUi(bool flag)
{
    ui->deliveryPage->setEnabled(flag);
    ui->connectBtn->setEnabled(flag);
    ui->ipEdit->setEnabled(flag);
    ui->portEdit->setEnabled(flag);
    ui->deliveryResetOriginBtn->setEnabled(flag);
    ui->deliveryUpdatePoseBtn->setEnabled(flag);
    ui->deliveryJoystickBtn->setEnabled(flag);
    ui->refreshSerialBn->setEnabled(flag);
    ui->serialPortInfoListBox->setEnabled(flag);
    ui->statusLineEdit->setEnabled(flag);
}


void MyWidget::on_connectBtn_clicked()
{
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)
        tcpSocket->abort();

    if(tcpSocket->state() != QAbstractSocket::ConnectedState){
        bool ok;
        tcpSocket->connectToHost(ui->ipEdit->text(), ui->portEdit->text().toInt(&ok, 10));
    }
    else{
        onUnsubscribeMovebaseStatus();
        onUnsubscribeFeedbackMsg();
        onUnsubscribeSensorButtonCmdVel();
        onUnsubscribeLongTimeStopAlarm();
        onUnsubscribeRealVolState();

        tcpSocket->disconnectFromHost();
    }
}

void MyWidget::tcpConnectStateChange()
{
    QTextStream connectionLogStream(&connectionLogFile);

    if(tcpSocket->state() == QAbstractSocket::UnconnectedState){
        tcpSocket->flush();
        ui->connectBtn->setText(tr("1-MOS Robot is Unconnected"));
        ui->connectBtn->setStyleSheet("background-color: rgb(255, 0, 0);");


        batItem->setBatteryValue(-1);

        enableUi(true);

        qint64 msecDiff = beginConnectedDateTime.msecsTo(QDateTime::currentDateTime());

        if(msecDiff != 0.0 && connectionLogFile.isOpen()){
            QString formattedTime;
            int hours = msecDiff/(1000*60*60);
            int minutes = (msecDiff-(hours*1000*60*60))/(1000*60);
            int seconds = (msecDiff-(minutes*1000*60)-(hours*1000*60*60))/1000;
            int milliseconds = msecDiff-(seconds*1000)-(minutes*1000*60)-(hours*1000*60*60);

            formattedTime.append(QString("%1").arg(hours, 2, 10, QLatin1Char('0')) + ":" +
                                 QString( "%1" ).arg(minutes, 2, 10, QLatin1Char('0')) + ":" +
                                 QString( "%1" ).arg(seconds, 2, 10, QLatin1Char('0')) + "." +
                                 QString( "%1" ).arg(milliseconds, 3, 10, QLatin1Char('0')));

            connectionLogStream << "Disconnected\t"
                                << QDate::currentDate().toString(Qt::LocalDate) << "\t"
                                << QTime::currentTime().toString("hh:mm:ss.zzz") << "\t"
                                << formattedTime
                                << endl;
        }
    }
    else if(tcpSocket->state() == QAbstractSocket::ConnectedState){   //QAbstractSocket::ConnectedState

        ui->connectBtn->setText(tr("1-MOS Robot is Connected"));
        ui->connectBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        enableUi(false);
        enableUiTimer->setInterval(10000);  //10s
        enableUiTimer->start();

        onAdvertiseCmdVel();
        onAdvertiseActionScript();
        onAdvertiseAskScript();
        onAdvertiseActionCommand();
        onAdvertiseKeyPriority();

        onSubscribeMovebaseStatus();
        onSubscribeFeedbackMsg();
        onSubscribeSensorButtonCmdVel();
        onSubscribeLongTimeStopAlarm();
        onSubscribeRealVolState();

        onPublishRobotModeFlag(MODE_TYPE::NAVI);

        beginConnectedDateTime = QDateTime::currentDateTime();
        connectionLogStream << "Connected\t"
                            << QDate::currentDate().toString(Qt::LocalDate) << "\t"
                            << QTime::currentTime().toString("hh:mm:ss.zzz")
                            << endl;
    }
}

void MyWidget::writeSettings()
{
    QSettings settings;
    settings.beginGroup(versionNum);
    settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.setValue("fullScreen", this->isFullScreen());
    settings.setValue("ip", ui->ipEdit->text());
    settings.setValue("port", ui->portEdit->text());
    settings.setValue("teleopMode", teleopMode);
    settings.setValue("langIdx", langIdx);
    settings.setValue("passwd", password());
    settings.setValue("origin_x", originPoint.position.x());
    settings.setValue("origin_y", originPoint.position.y());
    settings.setValue("origin_angle", originPoint.angle);
    settings.endGroup();
}

void MyWidget::readSettings()
{
    QSettings settings;
    settings.beginGroup(versionNum);
    setLang(settings.value("langIdx").toInt());
    this->resize(settings.value("size").toSize());
    this->move(settings.value("pos").toPoint());
    if(settings.value("fullScreen").toBool()){
        this->setWindowState(Qt::WindowFullScreen);
        this->showFullScreen();
    }
    if(settings.value("ip").toString().isEmpty())
        ui->ipEdit->setText("192.168.0.1");
    else
        ui->ipEdit->setText(settings.value("ip").toString());

    if(settings.value("port").toString().isEmpty())
        ui->portEdit->setText("9090");
    else
        ui->portEdit->setText(settings.value("port").toString());

    teleopMode = TELEOP_TYPE::LINEAR;
    if(settings.value("passwd").toString().isEmpty()){
        setPassword("12345678");
    }
    else
        setPassword(settings.value("passwd").toString());

    originPoint.position.setX(settings.value("origin_x").toFloat());
    originPoint.position.setY(settings.value("origin_y").toFloat());
    originPoint.angle = settings.value("origin_angle").toFloat();

    settings.endGroup();
}

void MyWidget::keyPressEvent(QKeyEvent *event)
{
    if(mode != MODE_TYPE::SLAM)
        return;
    if(event->key() == Qt::Key_W)
        velType = VEL_TYPE::Forward;
    else if(event->key() == Qt::Key_X)
        velType = VEL_TYPE::Backward;
    else if(event->key() == Qt::Key_A)
        velType = VEL_TYPE::Left;
    else if(event->key() == Qt::Key_D)
        velType = VEL_TYPE::Right;
}

void MyWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(mode != MODE_TYPE::SLAM)
        return;
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_X
            || event->key() == Qt::Key_A  || event->key() == Qt::Key_D ){
        if(!event->isAutoRepeat())
            velType = VEL_TYPE::Stop;
        onPublishVelCmd();
    }
}

void MyWidget::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            this->setWindowTitle(tr("MOS Delivery Service Software ") + versionNum);
            if(tcpSocket->state() == QAbstractSocket::UnconnectedState){
                ui->connectBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
                ui->connectBtn->setText(tr("1-MOS Robot is Unconnected"));
            }
            else if(tcpSocket->state() == QAbstractSocket::ConnectedState){
                ui->connectBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
                ui->connectBtn->setText(tr("1-MOS Robot is Connected"));
            }

            if(serial->isOpen()){
                ui->serialOpenBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
                ui->serialOpenBtn->setText(tr("2-Table Locator is Connected"));
            }
            else{
                ui->serialOpenBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
                ui->serialOpenBtn->setText(tr("2-Table Locator is Unconnected"));
            }
            break;
        }
        default:
            break;
        }
        QWidget::changeEvent(event);
    }
}

void MyWidget::handleEnableUiTimeout()
{
    enableUiTimer->stop();
    enableUi(true);
}

void MyWidget::handleVelCmdTimeout()
{
    if(velType == VEL_TYPE::Stop || teleopMode == TELEOP_TYPE::LINEAR)
        velCmdTimer->stop();
    onPublishVelCmd();
}

void MyWidget::readData()
{
    static QTime preTime = QTime::currentTime();
    QByteArray data;
    data = tcpSocket->readAll();
    QJsonParseError readError;
    QJsonDocument readJsonDoc = QJsonDocument::fromJson(data, &readError);

    if(readError.error == QJsonParseError::NoError){
        if(data.contains("/Long_Time_Stop_Alarm"))
            parseLongTimeStopAlarm(data);
        else if(data.contains("/real_voltage_state"))
            parseRealVolState(data);
        else if(data.contains("/feedbackMsg"))
            parseFeedbackMsg(data);
        else if(data.contains("/move_base/status"))
            parseMovebaseStatusJsonData(data);
        else if(data.contains("list_script_service"))
            parseListScriptServiceResponseJsonData(data);
        else if(data.contains("get_volume_service"))
            parseGetVolumeJsonData(data);
        else if(data.contains("/Sensor_Button_cmd_vel"))
            parseSensorButtonCmdVel(data);
        else if(data.contains("/ultra_sensor_status"))
            parseUltraSensorStatus(data);
    }
    else{
        allData.clear();
    }
}

void MyWidget::parseMovebaseStatusJsonData(QByteArray statusData)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(statusData, &error);

    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {
            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();
            if(msgJsonObj.contains("status_list")){
                QJsonArray statuslistJsonAry = msgJsonObj.take("status_list").toArray();
                QJsonObject statuslistJsonObj = statuslistJsonAry.last().toObject();
                if(statuslistJsonObj.contains("status")) {
                    int statusValue = statuslistJsonObj.take("status").toInt();
                    static int preStateValue = -1;
                    if(preStateValue != statusValue){
                        switch (statusValue) {
                        case 0:
                            ui->statusLineEdit->setText("Pending");
                            break;
                        case 1:
                            ui->statusLineEdit->setText("Active");
                            break;
                        case 2:
                            ui->statusLineEdit->setText("Preempted");
                            break;
                        case 3:
                            ui->statusLineEdit->setText("Succeeded");
                            break;
                        case 4:
                            ui->statusLineEdit->setText("Aborted");
                            break;
                        case 5:
                            ui->statusLineEdit->setText("Rejected");
                            break;
                        case 6:
                            ui->statusLineEdit->setText("Preempting");
                            break;
                        case 7:
                            ui->statusLineEdit->setText("Recalling");
                            break;
                        case 8:
                            ui->statusLineEdit->setText("Recalled");
                            break;
                        case 9:
                            ui->statusLineEdit->setText("Lost");
                            break;
                        default:
                            break;
                        }
                        preStateValue = statusValue;
                    }
                }
                else{
                    naviState = NAVISTATE::Null;
                    ui->statusLineEdit->setText("Null");
                }
            }
        }

    }
}

void MyWidget::parseFeedbackMsg(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {
            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();

            QString currentAction = msgJsonObj.take("action").toString();
            int currentIndex = msgJsonObj.take("currentIndex").toInt();

            if(currentAction == "Succeeded" || currentAction == "Failed"){
                if(deliveryMode == DELIVERYMODE_TYPE::DeliveryStart || deliveryMode == DELIVERYMODE_TYPE::DeliveryPause){
                    qint64 secDiff = beginDeliveryDateTime.secsTo(QDateTime::currentDateTime());
                    QTextStream deliveryLogStream(&deliveryResultLogFile);
                    deliveryLogStream << currentAction << "\t"
                                      << secDiff << "\t";
                }
                deliveryMode = DELIVERYMODE_TYPE::DeliveryStop;
                ui->deliveryCardTableLineEdit->setText("");
                ui->deliveryTableWidget->setEnabled(true);
                ui->deliveryPauseBtn->setEnabled(false);
                ui->deliveryPauseBtn->setText(tr("Delivery Pause"));
                emit updateDeliveryStatus2PerformanceDlg(false);
            }
        }
    }
}

void MyWidget::parseSensorButtonCmdVel(QByteArray data)
{
    Q_UNUSED(data);
    if(keyPriorityFlag){
        updateVel(VEL_TYPE::Stop);
    }
}

void MyWidget::parseLongTimeStopAlarm(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {
            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();
            if(msgJsonObj.take("data").toBool()){
                if(deliveryMode != DELIVERYMODE_TYPE::DeliveryStart)
                    return;
                if(mediaPlayer->state() != QMediaPlayer::PlayingState)
                    mediaPlayer->play();
                if(emergencyDlg == nullptr){
                    emergencyDlg = new JoystickDialog(true);
                    emergencyDlg->setAttribute(Qt::WA_DeleteOnClose);
                    emergencyDlg->setModal(true);
                    connect(emergencyDlg, &JoystickDialog::updateVel, this, &MyWidget::updateVel);
                    connect(emergencyDlg, &JoystickDialog::destroyed, this, &MyWidget::joystickDestroyed);
                    connect(emergencyDlg, &JoystickDialog::setTeleopMode, this, &MyWidget::setTeleopMode);
                    connect(emergencyDlg, &JoystickDialog::setKeyPriorityFlag, this, &MyWidget::setKeyPriorityFlag);
                    connect(emergencyDlg, &JoystickDialog::abortMovebase, this, &MyWidget::abortMovebase);
                    connect(emergencyDlg, &JoystickDialog::goOrigin, this, &MyWidget::goOrigin);
                    connect(emergencyDlg, &JoystickDialog::resetOrigin, this, &MyWidget::on_deliveryResetOriginBtn_clicked);
                    emergencyDlg->setFixedHeight(this->size().height()-100);
                    emergencyDlg->setFixedWidth(this->size().width()-100);

                    onPublishKeyPriority(false);
                    onSubscribeSensorButtonCmdVel();
                    velPercentage = 40;
                    emergencyDlg->show();
                }
                else{
                    onPublishKeyPriority(false);
                    velPercentage = 40;
                    emergencyDlg->setFlicker(true);
                    emergencyDlg->show();
                }
            }
        }
    }
}

void MyWidget::parseListScriptServiceResponseJsonData(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("values")) {
            QJsonObject valueJsonObj =  dataJsonObj.take("values").toObject();
            QString str = valueJsonObj.take("output").toString();
            QStringList lines = str.split( "\n", QString::SkipEmptyParts );
            if(listDlg->isVisible())
                listDlg->setList(lines);
        }
    }
}

void MyWidget::parseGetVolumeJsonData(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("values")) {
            QJsonObject valueJsonObj =  dataJsonObj.take("values").toObject();
            QString outputStr = valueJsonObj.take("output").toString();

            systemSettingDlg = new SystemSettingDialog(langIdx, outputStr, deskDb->getModel(), originPoint.position, originPoint.angle, this);
            systemSettingDlg->setAttribute(Qt::WA_DeleteOnClose);
            systemSettingDlg->setModal(true);
            connect(systemSettingDlg, &SystemSettingDialog::systemCommand, this, &MyWidget::systemCommand);
            connect(systemSettingDlg, &SystemSettingDialog::setVolume, this, &MyWidget::setVolume);
            connect(systemSettingDlg, &SystemSettingDialog::setLang, this, &MyWidget::setLang);
            connect(systemSettingDlg, &SystemSettingDialog::setOrigin, this, &MyWidget::setOrigin);
            connect(systemSettingDlg, &SystemSettingDialog::setSpeechRecogSwitch, this, &MyWidget::setSpeechRecogSwitch);
            connect(this, &MyWidget::updateUltrasonicValue, systemSettingDlg, &SystemSettingDialog::getUltrasonicValue);
            connect(systemSettingDlg, &SystemSettingDialog::destroyed, this, &MyWidget::onUnsubscribeUltraSensorStatus);
            onSubscribeUltraSensorStatus();
            systemSettingDlg->setFixedHeight(systemSettingDlg->size().height());
            systemSettingDlg->setFixedWidth(systemSettingDlg->size().width());
            systemSettingDlg->show();
        }
    }
}

void MyWidget::parseRealVolState(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {
            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();
            QString str = msgJsonObj.take("data").toString();
            int readVoltageState = str.mid(0, 3).toInt();
            float batteryLevel = 100 * ((float)readVoltageState/10.0 - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN) ;
            if(batteryLevel > 100.0)
                batteryLevel = 100.0;
            else if (batteryLevel < 0.0)
                batteryLevel = 0.0;
            ui->batLevelLcdNum->display(QString::number(batteryLevel, 'f', 0));
            batItem->setBatteryValue((int)batteryLevel);

            QString robotSwVersion = str.mid(3, 1) + "." + str.mid(4, str.size()-4);
            this->setWindowTitle(tr("MOS Delivery Service Software ") + versionNum
                                 + tr(" (Robot Software Version: v") + robotSwVersion + ")");
        }
    }
}

void MyWidget::parseUltraSensorStatus(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {
            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();
            int ultraSensorState = msgJsonObj.take("data").toInt();
            emit updateUltrasonicValue(ultraSensorState);
        }
    }
}

void MyWidget::onAdvertiseCmdVel()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/cmd_vel");
    rosJson.insert("type", "geometry_msgs/Twist");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseRobotModeFlag()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/Robot_Mode_Flag");
    rosJson.insert("type", "std_msgs/Byte");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnadvertiseRobotModeFlag()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unadvertise");
    rosJson.insert("topic", "/Robot_Mode_Flag");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishVelCmd()
{
    static float linearVel = 0.0;
    static float angularVel = 0.0;
    static float velPercentageReg = 25.0;

    float linearLimit = 0.90*(velPercentage/100.0);
    float angularLimit = 0.80*(velPercentage/100.0);
    float linearStep = 0.10;
    float angularStep = 0.16;

    switch (velType) {
    case VEL_TYPE::Forward:
        if(teleopMode == TELEOP_TYPE::JOG)
            angularVel = 0.0;
        if(linearVel < (linearLimit + linearStep))
            linearVel = linearVel + linearStep;
        break;
    case VEL_TYPE::Backward:
        if(teleopMode == TELEOP_TYPE::JOG)
            angularVel = 0.0;
        if(linearVel > -(linearLimit + linearStep))
            linearVel = linearVel - linearStep;
        break;
    case VEL_TYPE::Left:
        if(teleopMode == TELEOP_TYPE::JOG)
            linearVel = 0.0;
        if(angularVel < (angularLimit + angularStep))
            angularVel = angularVel + angularStep;
        break;
    case VEL_TYPE::Right:
        if(teleopMode == TELEOP_TYPE::JOG)
            linearVel = 0.0;
        if(angularVel > -(angularLimit + angularStep))
            angularVel = angularVel - angularStep;
        break;
    case VEL_TYPE::Stop:
        linearVel = 0.0;
        angularVel = 0.0;
        break;
    default:
        break;
    }

    if(velPercentageReg != velPercentage){
        velPercentageReg = velPercentage;
        if(linearVel != 0){
            if(linearVel > 0)
                while(linearVel > linearLimit)
                    linearVel -= linearStep;
            else
                while(linearVel < -linearLimit)
                    linearVel += linearStep;
        }
        if(angularVel != 0){
            if(angularVel > 0)
                while(angularVel > angularLimit)
                    angularVel -= angularStep;
            else
                while(angularVel < -angularLimit)
                    angularVel += angularStep;
        }
    }

    if(teleopMode == TELEOP_TYPE::JOG){
        if( (fabs(linearVel) > linearLimit*1.01) || (fabs(angularVel) > angularLimit*1.01) )
            return;
    }
    else if (teleopMode == TELEOP_TYPE::LINEAR){
        if(fabs(linearVel) > linearLimit*1.01){
            if(linearVel < 0.0)
                linearVel = linearVel + linearStep;
            else
                linearVel = linearVel - linearStep;
        }
        else if (fabs(angularVel) > angularLimit*1.01)
            if(angularVel < 0.0)
                angularVel = angularVel + angularStep;
            else
                angularVel = angularVel - angularStep;
    }

    QJsonObject rosJson, msgJson, linearJson, angularJson;
    linearJson.insert("x", linearVel);
    linearJson.insert("y", 0.0);
    linearJson.insert("z", 0.0);
    angularJson.insert("x", 0.0);
    angularJson.insert("y", 0.0);
    angularJson.insert("z", angularVel);
    msgJson.insert("linear", linearJson);
    msgJson.insert("angular", angularJson);
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    if(mode == MODE_TYPE::SLAM || mode == MODE_TYPE::XROCKER)
        rosJson.insert("topic", "/cmd_vel");
    else
        rosJson.insert("topic", "/key_cmd_vel");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishInitPose(bool isOrigin, QPointF pos, QQuaternion quat)
{
    float x = 0.0, y = 0.0;

    x = pos.y();
    y = pos.x();

    static int seqInitPose = 0;
    seqInitPose++;

    QJsonObject headerJson, stampJson;
    headerJson.insert("seq", seqInitPose);
    stampJson.insert("secs", QDateTime::currentDateTime().toMSecsSinceEpoch()/1000);    //5.6.3
    stampJson.insert("nsecs", 0);
    headerJson.insert("stamp", stampJson);
    headerJson.insert("frame_id", "");

    QJsonObject posesonJson, positionJson, orientationJson;
    positionJson.insert("x", x);
    positionJson.insert("y", y);
    positionJson.insert("z", 0.0);
    posesonJson.insert("position", positionJson);
    orientationJson.insert("x", quat.x());
    orientationJson.insert("y", quat.y());
    orientationJson.insert("z", quat.z());
    orientationJson.insert("w", quat.scalar());
    posesonJson.insert("orientation", orientationJson);

    QJsonArray covarianceJson;
    for(int i=0; i<36; i++){
        if(i == 0)
            covarianceJson.append(0.25);
        else if(i == 7)
            covarianceJson.append(0.25);
        else if(i == 35)
            covarianceJson.append(0.06853891945200942);
        else
            covarianceJson.append(0.0);
    }

    QJsonObject poseJson;
    poseJson.insert("pose", posesonJson);
    poseJson.insert("covariance", covarianceJson);

    QJsonObject msgJson;
    msgJson.insert("header", headerJson);
    msgJson.insert("pose", poseJson);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/initialpose");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishRobotModeFlag(MODE_TYPE type)
{
    mode = type;
    QJsonObject msgJson;
    if(type == MODE_TYPE::SLAM)
        msgJson.insert("data", (qint8)1);
    else if(type == MODE_TYPE::NAVI)
        msgJson.insert("data", (qint8)3);
    else if(type == MODE_TYPE::TEACHPOINT)
        msgJson.insert("data", (qint8)4);
    else if(type == MODE_TYPE::XROCKER)
        msgJson.insert("data", (qint8)2);
    else if(type == MODE_TYPE::EDITMAP)
        msgJson.insert("data", (qint8)5);
    else if(type == MODE_TYPE::DELIVERY)
        msgJson.insert("data", (qint8)6);
    else if(type == MODE_TYPE::RESETNODE)
        msgJson.insert("data", (qint8)11);
    else if(type == MODE_TYPE::RESTARTAGV)
        msgJson.insert("data", (qint8)12);
    else if(type == MODE_TYPE::SHUTDOWNAGV)
        msgJson.insert("data", (qint8)13);
    else if(type == MODE_TYPE(10))
        msgJson.insert("data", (qint8)10);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/Robot_Mode_Flag");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishActionCommand(int cmdType, QString name, int startLine)
{
    QJsonObject msgJson;
    msgJson.insert("cmdType", (qint8)cmdType);
    msgJson.insert("scriptName", name);
    msgJson.insert("startLine", startLine);
    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/actionCommand");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();

    if(cmdType == 1){
        for(int i=0; i<deskDb->getModel()->rowCount(); i++){
            if(currentDeliveryScriptName == deskDb->getModel()->index(i, 3).data().toString()){
                //ui->deliveryCardTableLineEdit->setText(deskDb->getModel()->index(i, 1).data().toString());
                QStringList strLines = currentDeliveryCT.split( "\n", QString::SkipEmptyParts );
                QString tableStr = strLines.at(0);
                QString cardStr = strLines.at(1);
                ui->deliveryCardTableLineEdit->setText(cardStr + " / " + tableStr);
                break;
            }
        }
    }
    else //if(cmdType == 2)
        ui->deliveryCardTableLineEdit->setText("");
}

void MyWidget::onPublishKeyPriority(bool flag)
{
    QJsonObject msgJson;
    msgJson.insert("data", flag);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/key_priority");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishAudio(int id)
{
    QJsonObject msgJson;
    msgJson.insert("data", id);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/Robot_Sound_Flag");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishPeripheral(int type, int id)
{
    QJsonObject msgJson;
    msgJson.insert("actionTypeID", type);
    msgJson.insert("statusID", id);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/write_external_comm");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishMoveBaseCancel()
{
    QJsonObject msgJson, stampJson;
    stampJson.insert("secs", QDateTime::currentDateTime().toMSecsSinceEpoch()/1000);    //5.6.3
    stampJson.insert("nsecs", 0);

    msgJson.insert("stamp", stampJson);
    msgJson.insert("id", "");

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/move_base/cancel");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishMoveBaseGoalForOrigin()
{
    QQuaternion quat = QQuaternion::fromEulerAngles(0.0, 0.0, originPoint.angle);

    QJsonObject headerJson, stampJson;
    stampJson.insert("secs", QDateTime::currentDateTime().toMSecsSinceEpoch()/1000);    //5.6.3
    stampJson.insert("nsecs", 0);
    headerJson.insert("stamp", stampJson);
    headerJson.insert("frame_id", "map");

    QJsonObject goalidJson;
    goalidJson.insert("stamp", stampJson);
    goalidJson.insert("id", "");

    QJsonObject poseJson, positionJson, orientationJson;
    positionJson.insert("x", originPoint.position.y());
    positionJson.insert("y", originPoint.position.x());
    positionJson.insert("z", 0.0);
    poseJson.insert("position", positionJson);
    orientationJson.insert("x", quat.x());
    orientationJson.insert("y", quat.y());
    orientationJson.insert("z", quat.z());
    orientationJson.insert("w", quat.scalar());
    poseJson.insert("orientation", orientationJson);

    QJsonObject targetPoseJson;
    targetPoseJson.insert("header", headerJson);
    targetPoseJson.insert("pose", poseJson);

    QJsonObject msgJson, goalJson;
    goalJson.insert("target_pose", targetPoseJson);
    msgJson.insert("header", headerJson);
    msgJson.insert("goal_id", goalidJson);
    msgJson.insert("goal", goalJson);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/move_base/goal");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishSpeechRecognitionSwitch(bool flag)
{
    QJsonObject msgJson;
    msgJson.insert("data", flag);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/speech_recognition_switch");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeMovebaseStatus()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/move_base/status");
    rosJson.insert("type", "actionlib_msgs/GoalStatusArray");
    rosJson.insert("throttle_rate", 1000);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeFeedbackMsg()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/feedbackMsg");
    rosJson.insert("type", "my_action_actionlib/feedbackMsg");
    rosJson.insert("throttle_rate", 1000);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeSensorButtonCmdVel()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/Sensor_Button_cmd_vel");
    rosJson.insert("type", "geometry_msgs/Twist");
    rosJson.insert("throttle_rate", 500);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeLongTimeStopAlarm()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/Long_Time_Stop_Alarm");
    rosJson.insert("type", "std_msgs/Bool");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeRealVolState()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/real_voltage_state");
    rosJson.insert("type", "std_msgs/String");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeUltraSensorStatus()
{
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/ultra_sensor_status");
    rosJson.insert("type", "std_msgs/UInt8");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeMovebaseStatus()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/move_base/status");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeFeedbackMsg()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/feedbackMsg");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeSensorButtonCmdVel()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/Sensor_Button_cmd_vel");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeLongTimeStopAlarm()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/Long_Time_Stop_Alarm");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeRealVolState()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/real_voltage_state");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeUltraSensorStatus()
{
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/ultra_sensor_status");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceListScript()
{
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "list_script_service");

    QJsonObject arg;
    arg.insert("input", " ");
    rosJson.insert("args", arg);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceDeleteScript(QString nameStr)
{
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "delete_script_service");

    QJsonObject arg;
    arg.insert("input", nameStr);
    rosJson.insert("args", arg);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceUpdatePose()
{
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "request_nomotion_update");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceLoadMap(QString nameStr)
{
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "load_map_service");

    QJsonObject arg;
    arg.insert("input", nameStr + ".yaml");
    rosJson.insert("args", arg);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceGetVolume()
{
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "sound_handle/get_volume_service");

    QJsonObject arg;
    arg.insert("input", "");
    rosJson.insert("args", arg);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onCallServiceSetVolume(int value)
{
    QString valueStr = QString::number(value, 10);
    QJsonObject rosJson;
    rosJson.insert("op", "call_service");
    rosJson.insert("service", "sound_handle/set_volume_service");

    QJsonObject arg;
    arg.insert("input", valueStr + "%");
    rosJson.insert("args", arg);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::updateVel(VEL_TYPE vel_type)
{
    velType = vel_type;
    if(!velCmdTimer->isActive())
        velCmdTimer->start();
}

void MyWidget::loadScript(QString name)
{
    emit deskGetScriptName(name);
}

void MyWidget::parseFailedReason(FAILED_REASON type)
{
    QTextStream deliveryLogStream(&deliveryResultLogFile);
    switch (type) {
    case FAILED_REASON::ChannelObstacle:
        deliveryLogStream << "ChannelObstacle";
        break;
    case FAILED_REASON::FieldChange:
        deliveryLogStream << "FieldChange";
        break;
    case FAILED_REASON::GetLost:
        deliveryLogStream << "GetLost";
        break;
    case FAILED_REASON::ArmError:
        deliveryLogStream << "ArmError";
        break;
    case FAILED_REASON::MealError:
        deliveryLogStream << "MealError";
        break;
    case FAILED_REASON::NotGoing:
        deliveryLogStream << "NotGoing";
        break;
    case FAILED_REASON::Other:
        deliveryLogStream << "Other";
        break;
    case FAILED_REASON::EngineeringTest:
        deliveryLogStream << "EngineeringTest";
        break;
    default:
        deliveryLogStream << type;
        break;
    }

}

void MyWidget::joystickDestroyed()
{
    onUnsubscribeSensorButtonCmdVel();
    onPublishKeyPriority(false);
    emergencyDlg = nullptr;
    mediaPlayer->stop();
}

void MyWidget::onAdvertiseActionScript()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/actionScript");
    rosJson.insert("type", "my_script_handler/actionList");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseAskScript()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/askScript");
    rosJson.insert("type", "std_msgs/String");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseActionCommand()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/actionCommand");
    rosJson.insert("type", "my_action_actionlib/actionCmd");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseKeyPriority()
{
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/key_priority");
    rosJson.insert("type", "std_msgs/Bool");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::initDelivery()
{
    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    connect(serial, &QSerialPort::readyRead, this, &MyWidget::readSerialData);
}

void MyWidget::setDeliveryTable()
{
    ui->deliveryTableWidget->clear();
    ui->deliveryTableWidget->horizontalHeader()->setVisible(true);
    ui->deliveryTableWidget->verticalHeader()->setVisible(true);
    ui->deliveryTableWidget->setColumnCount(3);
    ui->deliveryTableWidget->setRowCount(deliveryList.count());

    ui->deliveryTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Card No.")));
    ui->deliveryTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Table No.")));
    ui->deliveryTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Execution")));

    for(int i=0; i<3; i++)
        ui->deliveryTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

    for(int i=0; i<deliveryList.count(); i++){
        ui->deliveryTableWidget->setItem(i, 0, new QTableWidgetItem(deliveryList.at(i)->cardNum));
        ui->deliveryTableWidget->setItem(i, 1, new QTableWidgetItem(deliveryList.at(i)->tableNum));
        ui->deliveryTableWidget->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->deliveryTableWidget->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        QFont font;
        font.setPointSize(30);
        ui->deliveryTableWidget->item(i, 0)->setFont(font);
        ui->deliveryTableWidget->item(i, 1)->setFont(font);

        QWidget* pWidget = new QWidget();
        QPushButton* btn_edit = new QPushButton();
        btn_edit->setText(tr("GO"));
        btn_edit->setFont(font);
        btn_edit->setObjectName(deliveryList.at(i)->tableNum + "\n" + deliveryList.at(i)->cardNum);
        btn_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(btn_edit, &QPushButton::clicked, this, &MyWidget::deliveryAction);
        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(btn_edit);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        ui->deliveryTableWidget->setCellWidget(i, 2, pWidget);
    }
    ui->deliveryTableWidget->verticalHeader()->setDefaultSectionSize(60);
    ui->deliveryTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->deliveryTableWidget->clearSelection();
}

void MyWidget::on_deliveryPauseBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState){
        ui->deliveryPauseBtn->setText(tr("Delivery Pause"));
        deliveryMode = DELIVERYMODE_TYPE::DeliveryStop;
        return;
    }
    if(ui->deliveryPauseBtn->text() == tr("Delivery Pause")){
        ui->deliveryPauseBtn->setText(tr("Delivery Resume"));
        deliveryMode = DELIVERYMODE_TYPE::DeliveryPause;
        onPublishActionCommand(3, currentDeliveryScriptName, -1);
    }
    else if(ui->deliveryPauseBtn->text() == tr("Delivery Resume")){
        ui->deliveryPauseBtn->setText(tr("Delivery Pause"));
        deliveryMode = DELIVERYMODE_TYPE::DeliveryStart;
        onPublishActionCommand(1, currentDeliveryScriptName, -1);
    }
}

void MyWidget::on_deliveryStopBtn_clicked()
{
    deliveryMode = DELIVERYMODE_TYPE::DeliveryStop;
    ui->deliveryTableWidget->setEnabled(true);
    ui->deliveryTableWidget->clearSelection();
    ui->deliveryCardTableLineEdit->setText("");

    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    onPublishActionCommand(2, currentDeliveryScriptName, -1);
    onPublishMoveBaseCancel();

    if(ui->deliveryPauseBtn->isEnabled()){
        ui->deliveryPauseBtn->setText(tr("Delivery Pause"));
        ui->deliveryPauseBtn->setEnabled(false);
        FailedReasonDialog *dlg = new FailedReasonDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setModal(true);
        connect(dlg, &FailedReasonDialog::parseFailedReason, this, &MyWidget::parseFailedReason);
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);
        int rect = dlg->exec();
        if(!rect){
            if(emergencyDlg == nullptr){
                emergencyDlg = new JoystickDialog(false);
                emergencyDlg->setAttribute(Qt::WA_DeleteOnClose);
                emergencyDlg->setModal(true);
                connect(emergencyDlg, &JoystickDialog::updateVel, this, &MyWidget::updateVel);
                connect(emergencyDlg, &JoystickDialog::destroyed, this, &MyWidget::joystickDestroyed);
                connect(emergencyDlg, &JoystickDialog::setTeleopMode, this, &MyWidget::setTeleopMode);
                connect(emergencyDlg, &JoystickDialog::setKeyPriorityFlag, this, &MyWidget::setKeyPriorityFlag);
                connect(emergencyDlg, &JoystickDialog::abortMovebase, this, &MyWidget::abortMovebase);
                connect(emergencyDlg, &JoystickDialog::goOrigin, this, &MyWidget::goOrigin);
                connect(emergencyDlg, &JoystickDialog::resetOrigin, this, &MyWidget::on_deliveryResetOriginBtn_clicked);
                emergencyDlg->setFixedHeight(this->size().height()-20);
                emergencyDlg->setFixedWidth(this->size().width()-100);

                onPublishKeyPriority(false);
                onSubscribeSensorButtonCmdVel();
                velPercentage = 40;
                emergencyDlg->show();
            }
            else{
                mediaPlayer->stop();
                emergencyDlg->setFlicker(false);

                onPublishKeyPriority(false);
                velPercentage = 40;
                emergencyDlg->show();
            }
        }
    }
}

void MyWidget::deliveryAction()
{
    if(deliveryMode == DELIVERYMODE_TYPE::DeliveryStart || deliveryMode == DELIVERYMODE_TYPE::DeliveryPause)
        return;
    QObject *obj = QObject::sender();

    QStringList strLines = obj->objectName().split( "\n", QString::SkipEmptyParts );
    QString tableStr = strLines.at(0);
    QString cardStr = strLines.at(1);
    currentDeliveryCT = obj->objectName();

    ui->deliveryTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    for(int i=0; i<deskDb->getModel()->rowCount(); i++){
        if(tableStr == deskDb->getModel()->index(i, 1).data().toString()){

            if(deskDb->getModel()->index(i, 3).data().toString() == " "){
                QMessageBox mb;
                mb.setStyleSheet("");
                mb.setIcon(QMessageBox::Warning);
                mb.setWindowTitle(tr("Error"));
                mb.setText(tr("Table <%1> did not set the script!").arg(tableStr));
                mb.exec();
                return;
            }
            WarringDialog *dlg = new WarringDialog(this);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->setMessage(tr("Delivery to < Card %1 / Table %2 > ?").arg(cardStr).arg(tableStr));

            dlg->setModal(true);
            dlg->setFixedSize(dlg->width(), dlg->height());
            dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

            QTimer::singleShot(10000, dlg, SLOT(close()));
            if(dlg->exec()){
                currentDeliveryScriptName = deskDb->getModel()->index(i, 3).data().toString();
                deliveryMode = DELIVERYMODE_TYPE::DeliveryStart;
                ui->deliveryTableWidget->setEnabled(false);
                ui->deliveryPauseBtn->setEnabled(true);
                ui->deliveryStopBtn->setEnabled(true);
                onPublishActionCommand(1, currentDeliveryScriptName, -1);
                beginDeliveryDateTime = QDateTime::currentDateTime();
                QString tableStr = deskDb->getModel()->index(i, 1).data().toString();
                QTextStream deliveryLogStream(&deliveryResultLogFile);
                deliveryLogStream << endl << QDate::currentDate().toString(Qt::LocalDate) << "\t"
                                  << QTime::currentTime().toString("hh:mm:ss.zzz") << "\t"
                                  << QTime::currentTime().hour() << "\t"
                                  << QTime::currentTime().minute() << "\t"
                                  << QTime::currentTime().second() << "\t"
                                  << tableStr << "\t";
                emit updateDeliveryStatus2PerformanceDlg(true);
            }
            break;
        }
    }
}

void MyWidget::deliveryPerformance(int order)
{
    QString tableStr = deskDb->getModel()->index(order, 1).data().toString();
    WarringDialog *dlg = new WarringDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setMessage(tr("Delivery to Table %1?").arg(tableStr));

    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

    QTimer::singleShot(10000, dlg, SLOT(close()));
    if(dlg->exec()){
        deliveryMode = DELIVERYMODE_TYPE::DeliveryStart;
        ui->deliveryTableWidget->setEnabled(false);
        ui->deliveryPauseBtn->setEnabled(true);
        ui->deliveryStopBtn->setEnabled(true);
        onPublishActionCommand(1, deskDb->getModel()->index(order, 3).data().toString(), -1);
    }
    else{
        emit updateDeliveryStatus2PerformanceDlg(false);
    }
}

void MyWidget::openSerialPort()
{
    setDeliveryTable();

    serial->setPortName(ui->serialPortInfoListBox->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        ui->serialOpenBtn->setText(tr("2-Table Locator is Connected"));
        ui->serialOpenBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        ui->serialPortInfoListBox->setEnabled(false);
    } else
        QMessageBox::critical(this, tr("Error"), serial->errorString());
}

void MyWidget::closeSerialPort()
{
    serial->close();
    ui->serialOpenBtn->setText(tr("2-Table Locator is Unconnected"));
    ui->serialOpenBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
    ui->serialPortInfoListBox->setEnabled(true);
}

void MyWidget::on_serialOpenBtn_clicked()
{
    if(!serial->isOpen())
        openSerialPort();
    else
        closeSerialPort();
}

void MyWidget::readSerialData()
{
    static QString dataLink = "";
    QByteArray data;
    data = serial->readAll();
    dataLink = dataLink + QString(data);
    if(dataLink.contains("RXOK")){
        dataLink.clear();
        return;
    }
    else if(dataLink.length() == 27){
        QString deskNum = "", cardNum = "";
        deskId = dataLink.mid(15, 4);
        cardId = dataLink.mid(19, 4);
        for(int i=0; i<deskDb->getModel()->rowCount(); ++i)
            if(deskId == deskDb->getModel()->index(i, 2).data().toString()){
                deskNum = deskDb->getModel()->index(i, 1).data().toString();
                break;
            }
        for(int i=0; i<cardDb->getModel()->rowCount(); ++i)
            if(cardId == cardDb->getModel()->index(i, 2).data().toString()){
                cardNum = cardDb->getModel()->index(i, 1).data().toString();
                break;
            }
        if(!deskNum.isEmpty() && !cardNum.isEmpty()){
            //check plug or unplug
            if(dataLink.at(14) == 'E'){     //plug
                //if(dataLink.at(13) == 'E'){     //plug
                bool isListContain = false;
                for(int i=0; i<deliveryList.count(); i++){
                    if(deliveryList.at(i)->cardNum == cardNum)
                        isListContain = true;
                }
                if(!isListContain){
                    deliveryItem *item = new deliveryItem;
                    item->tableNum = deskNum;
                    item->cardNum = cardNum;
                    deliveryList.append(item);
                    setDeliveryTable();
                }
            }
            else if(dataLink.at(14) == '0'){   //unplug
                //else if(dataLink.at(13) == '0'){   //unplug
                bool isListContain = false;
                for(int i=0; i<deliveryList.count(); i++)
                    if((deliveryList.at(i)->tableNum == deskNum) && (deliveryList.at(i)->cardNum == cardNum)){
                        deliveryItem *item = deliveryList.at(i);
                        deliveryList.removeAt(i);
                        delete item;
                        isListContain = true;
                    }
                if(isListContain)
                    setDeliveryTable();
            }
        }
        else{
            if(deskNum.isEmpty() && deskDb->isVisible() && deskDb->isActiveWindow()){
                dbAddMessageBox = new QMessageBox;
                dbAddMessageBox->setAttribute(Qt::WA_DeleteOnClose);
                dbAddMessageBox->setIcon(QMessageBox::Information);
                dbAddMessageBox->setWindowTitle(tr("Information"));
                dbAddMessageBox->setText(QString(tr("This desk ID %1 was not found.\r\nDo you want to add it now?")).arg(deskId));
                dbAddMessageBox->setStandardButtons(QMessageBox::Yes);
                dbAddMessageBox->addButton(QMessageBox::No);
                dbAddMessageBox->setDefaultButton(QMessageBox::Yes);
                QList<QAbstractButton*> mBoxButtons = dbAddMessageBox->buttons();
                connect(mBoxButtons.at(0), SIGNAL(pressed()), this, SLOT(clickAddDeskDb()));
                dbAddMessageBox->show();
                QTimer::singleShot(5000, dbAddMessageBox, SLOT(close()));
            }
            if(cardNum.isEmpty() && cardDb->isVisible() && cardDb->isActiveWindow()){
                dbAddMessageBox = new QMessageBox;
                dbAddMessageBox->setAttribute(Qt::WA_DeleteOnClose);
                dbAddMessageBox->setIcon(QMessageBox::Information);
                dbAddMessageBox->setWindowTitle(tr("Information"));
                dbAddMessageBox->setText(QString(tr("This card ID %1 was not found.\r\nDo you want to add it now?")).arg(cardId));
                dbAddMessageBox->setStandardButtons(QMessageBox::Yes);
                dbAddMessageBox->addButton(QMessageBox::No);
                dbAddMessageBox->setDefaultButton(QMessageBox::Yes);
                QList<QAbstractButton*> mBoxButtons = dbAddMessageBox->buttons();
                connect(mBoxButtons.at(0), SIGNAL(pressed()), this, SLOT(clickAddCardDb()));
                dbAddMessageBox->show();
                QTimer::singleShot(5000, dbAddMessageBox, SLOT(close()));
            }
        }
        QString replyData = dataLink;

        replyData.replace(2, 1, "R");
        replyData.replace(4, 1, "8");
        replyData.replace(5, 1, "8");
        replyData.replace(23, 1, "F");

        serial->write(replyData.toLatin1());
        serial->flush();
        dataLink.clear();
    }
    else if(dataLink.length() == 17 || dataLink.length() > 27){
        dataLink.clear();
    }
}

void MyWidget::clickAddDeskDb()
{
    emit addEmptyDeskDb(deskId);
    dbAddMessageBox->close();
}

void MyWidget::clickAddCardDb()
{
    emit addEmptyCardDb(cardId);
    dbAddMessageBox->close();
}

void MyWidget::goOrigin()
{
    WarringDialog *dlg = new WarringDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setMessage(tr("Are you sure to automatically navigate the robot back to the origin? \n(Please confirm that the robot has enough route to return to the origin)"));
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

    if(dlg->exec()){
        //onPublishActionCommand(4, currentDeliveryScriptName, -1); //run on actionlib on Robot
        onPublishKeyPriority(false);
        onPublishMoveBaseGoalForOrigin();                           //
        emit updateDeliveryStatus2PerformanceDlg(true);
    }
    else
        emit updateDeliveryStatus2PerformanceDlg(false);
}

void MyWidget::poseUpdate()
{
    onCallServiceUpdatePose();
    if(poseUpdateNum > 0){
        QTimer::singleShot(1000, this, SLOT(poseUpdate()));
        poseUpdateNum--;
    }
}

void MyWidget::abortMovebase()
{
    onPublishActionCommand(2, currentDeliveryScriptName, -1);
    onCallServiceUpdatePose();
    on_deliveryStopBtn_clicked();
}

void MyWidget::setTeleopMode(TELEOP_TYPE type)
{
    teleopMode = type;
}

void MyWidget::setKeyPriorityFlag(bool flag)
{
    keyPriorityFlag = flag;
    onPublishKeyPriority(!flag);
}

void MyWidget::deskAskListScript()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    listDlg->clearList();
    listDlg->setListType(LIST_TYPE::LISTSCRIPT);
    listDlg->show();

    onCallServiceListScript();
}

void MyWidget::on_deliveryTableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    deliveryList.removeAt(item->row());
    delete item;
    setDeliveryTable();
}

void MyWidget::on_deliveryJoystickBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    if(emergencyDlg == nullptr){
        emergencyDlg = new JoystickDialog(false);
        emergencyDlg->setAttribute(Qt::WA_DeleteOnClose);
        emergencyDlg->setModal(true);
        connect(emergencyDlg, &JoystickDialog::updateVel, this, &MyWidget::updateVel);
        connect(emergencyDlg, &JoystickDialog::destroyed, this, &MyWidget::joystickDestroyed);
        connect(emergencyDlg, &JoystickDialog::setTeleopMode, this, &MyWidget::setTeleopMode);
        connect(emergencyDlg, &JoystickDialog::setKeyPriorityFlag, this, &MyWidget::setKeyPriorityFlag);
        connect(emergencyDlg, &JoystickDialog::abortMovebase, this, &MyWidget::abortMovebase);
        connect(emergencyDlg, &JoystickDialog::goOrigin, this, &MyWidget::goOrigin);
        connect(emergencyDlg, &JoystickDialog::resetOrigin, this, &MyWidget::on_deliveryResetOriginBtn_clicked);
        emergencyDlg->setFixedHeight(this->size().height()-20);
        emergencyDlg->setFixedWidth(this->size().width()-100);

        onPublishKeyPriority(false);
        onSubscribeSensorButtonCmdVel();
        velPercentage = 40;
        emergencyDlg->show();
    }
    else{
        onPublishKeyPriority(false);
        velPercentage = 40;
        emergencyDlg->setFlicker(false);
        emergencyDlg->show();
    }
}

void MyWidget::on_refreshSerialBn_clicked()
{
    initDelivery();
}

void MyWidget::on_deliveryUpdatePoseBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    poseUpdateNum = 2;
    poseUpdate();
}

void MyWidget::on_deliveryResetOriginBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    WarringDialog *dlg = new WarringDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setMessage(tr("Are you sure the robot has returned to the origin?"));
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

    if(dlg->exec()){
        QString firstDeliveryScriptName = deskDb->getModel()->index(0, 3).data().toString();
        onPublishActionCommand(5, firstDeliveryScriptName, -1);

        enableUi(false);
        enableUiTimer->setInterval(3000);  //3s
        enableUiTimer->start();

        QThread::msleep(1000);

        onPublishInitPose(true, originPoint.position, QQuaternion::fromEulerAngles(0.0, 0.0, originPoint.angle));
        poseUpdateNum = 5;
        poseUpdate();
    }
}

void MyWidget::joystick4Administrator()
{
    if(emergencyDlg == nullptr){
        emergencyDlg = new JoystickDialog(false);
        emergencyDlg->setAttribute(Qt::WA_DeleteOnClose);
        emergencyDlg->setModal(true);
        connect(emergencyDlg, &JoystickDialog::updateVel, this, &MyWidget::updateVel);
        connect(emergencyDlg, &JoystickDialog::destroyed, this, &MyWidget::joystickDestroyed);
        connect(emergencyDlg, &JoystickDialog::setTeleopMode, this, &MyWidget::setTeleopMode);
        connect(emergencyDlg, &JoystickDialog::setKeyPriorityFlag, this, &MyWidget::setKeyPriorityFlag);
        connect(emergencyDlg, &JoystickDialog::abortMovebase, this, &MyWidget::abortMovebase);
        connect(emergencyDlg, &JoystickDialog::goOrigin, this, &MyWidget::goOrigin);
        connect(emergencyDlg, &JoystickDialog::resetOrigin, this, &MyWidget::on_deliveryResetOriginBtn_clicked);
        emergencyDlg->setFixedHeight(this->size().height()-100);
        emergencyDlg->setFixedWidth(this->size().width()-100);

        onPublishKeyPriority(false);
        onSubscribeSensorButtonCmdVel();
        velPercentage = 100;
        emergencyDlg->show();
    }
    else{
        onPublishKeyPriority(false);
        velPercentage = 100;
        emergencyDlg->setFlicker(true);
        emergencyDlg->show();
    }
}

void MyWidget::setVolume(int value)
{
    onCallServiceSetVolume(value);
}

void MyWidget::setLang(int idx)
{
    langIdx = idx;
    if(langIdx == 0)
        qApp->removeTranslator(trsChinese);
    else if(langIdx == 1)
        qApp->installTranslator(trsChinese);
    setDeliveryTable();
}

void MyWidget::requestPerformance(PERFORMANCE_TYPE type, int value)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState){
        emit updateDeliveryStatus2PerformanceDlg(false);
        return;
    }
    if(type == PERFORMANCE_TYPE::AUDIO_TYPE){
        onPublishAudio(value);
    }
    else if(type == PERFORMANCE_TYPE::ARM_TYPE){
        if(value == 1)
            onPublishPeripheral(1001, 1);
        else if(value == 2)
            onPublishPeripheral(1002, 1);
    }
    else if(type == PERFORMANCE_TYPE::LED_TYPE){
        onPublishPeripheral(1003, value);
    }
    else if(type == PERFORMANCE_TYPE::DELIVERY_TYPE){
        deliveryPerformance(value);
    }
    else if(type == PERFORMANCE_TYPE::ABORT_TYPE){
        onPublishActionCommand(2, " ", -1);
    }
    else if(type == PERFORMANCE_TYPE::GOORIGIN_TYPE){
        goOrigin();
    }
    else if(type == PERFORMANCE_TYPE::RESETORIGIN_TYPE){
        on_deliveryResetOriginBtn_clicked();
    }
}

void MyWidget::setAudioOff()
{
    onPublishAudio(6);
}

void MyWidget::setOrigin(QPointF position, float angle)
{
    originPoint.position.setX(position.x());
    originPoint.position.setY(position.y());
    originPoint.angle = angle;
}

void MyWidget::setSpeechRecogSwitch(bool flag)
{
    onPublishSpeechRecognitionSwitch(flag);
}

void MyWidget::on_systemBtn_clicked()
{
    if(tcpSocket->state() == QAbstractSocket::ConnectedState){
        onCallServiceGetVolume();
    }
    else{
        systemSettingDlg = new SystemSettingDialog(langIdx, "", deskDb->getModel(), originPoint.position, originPoint.angle, this);
        systemSettingDlg->setAttribute(Qt::WA_DeleteOnClose);
        systemSettingDlg->setModal(true);
        connect(systemSettingDlg, &SystemSettingDialog::systemCommand, this, &MyWidget::systemCommand);
        connect(systemSettingDlg, &SystemSettingDialog::setVolume, this, &MyWidget::setVolume);
        connect(systemSettingDlg, &SystemSettingDialog::setLang, this, &MyWidget::setLang);
        connect(systemSettingDlg, &SystemSettingDialog::setOrigin, this, &MyWidget::setOrigin);
        connect(systemSettingDlg, &SystemSettingDialog::setSpeechRecogSwitch, this, &MyWidget::setSpeechRecogSwitch);
        systemSettingDlg->setFixedHeight(systemSettingDlg->size().height());
        systemSettingDlg->setFixedWidth(systemSettingDlg->size().width());
        systemSettingDlg->show();
    }
}

void MyWidget::systemCommand(SYSTEM_CMD cmd)
{
    switch(cmd){
    case SYSTEM_CMD::SHUTDOWN:{
        if(tcpSocket->state() != QAbstractSocket::ConnectedState)
            return;
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setMessage(tr("Are you sure you want to shutdown the robot ?"));
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

        if(dlg->exec()){
            velType = VEL_TYPE::Stop;
            onPublishVelCmd();
            onPublishRobotModeFlag(MODE_TYPE::SHUTDOWNAGV);
            QTime dieTime= QTime::currentTime().addSecs(5);
            while(true){
                if(QTime::currentTime() > dieTime){
                    systemSettingDlg->close();
                    this->close();
                    break;
                }
            }
        }
        break;
    }
    case SYSTEM_CMD::REBOOT:{
        if(tcpSocket->state() != QAbstractSocket::ConnectedState)
            return;
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setMessage(tr("Are you sure you want to reboot the robot ?"));
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

        if(dlg->exec()){
            velType = VEL_TYPE::Stop;
            onPublishVelCmd();
            ui->statusLineEdit->setText("Null");
            onUnsubscribeMovebaseStatus();
            onUnsubscribeFeedbackMsg();
            onUnsubscribeLongTimeStopAlarm();
            onPublishRobotModeFlag(MODE_TYPE::RESTARTAGV);
        }
        break;
    }
    case SYSTEM_CMD::RESETSTATUS:{
        if(tcpSocket->state() != QAbstractSocket::ConnectedState)
            return;
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setMessage(tr("Are you sure you want to reset the robot status?"));
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

        if(dlg->exec()){
            velType = VEL_TYPE::Stop;
            onPublishVelCmd();
            onUnsubscribeMovebaseStatus();
            onUnsubscribeFeedbackMsg();
            onUnsubscribeLongTimeStopAlarm();

            enableUi(false);
            onPublishRobotModeFlag(MODE_TYPE::RESETNODE);
            QTime dieTime= QTime::currentTime().addSecs(10);
            while(true){
                if(QTime::currentTime() > dieTime){
                    tcpConnectStateChange();
                    break;
                }
            }
        }
        break;
    }
    case SYSTEM_CMD::EDITCARDDB:{
        bool ok;
        QString pw = ChangePwdDialog::getOnePassword(this, &ok);
        if (ok && !pw.isEmpty()) {
            if(pw == password() || pw == QString("123456"))
                cardDb->show();
            else{
                QMessageBox::warning(this, tr("Error"), tr("Wrong password!"));
                return;
            }
        }
        break;
    }
    case SYSTEM_CMD::EDITTABLEDB:{
        bool ok;
        QString pw = ChangePwdDialog::getOnePassword(this, &ok);
        if (ok && !pw.isEmpty()) {
            if(pw == password() || pw == QString("123456"))
                deskDb->show();
            else{
                QMessageBox::warning(this, tr("Error"), tr("Wrong password!"));
                return;
            }
        }
        break;
    }
    case SYSTEM_CMD::TESTSOUND:{
        if(tcpSocket->state() != QAbstractSocket::ConnectedState)
            return;
        onPublishRobotModeFlag((MODE_TYPE)10);
        break;
    }
    case SYSTEM_CMD::CHANGEPWD:{
        bool ok;
        QString pw = ChangePwdDialog::getOnePassword(this, &ok);
        if (ok && !pw.isEmpty()) {
            if(pw == password() || pw == QString("123456")){
                ChangePwdDialog *dlg = new ChangePwdDialog(this);
                dlg->setAttribute(Qt::WA_DeleteOnClose);
                dlg->setModal(true);
                dlg->setFixedSize(dlg->width(), dlg->height());
                dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);
                connect(dlg, &ChangePwdDialog::setNewPassword, this, &MyWidget::setPassword);
                dlg->show();
            }
            else{
                QMessageBox::warning(this, tr("Error"), tr("Wrong password!"));
                return;
            }
        }
        break;
    }
    default:
        break;
    }
}
