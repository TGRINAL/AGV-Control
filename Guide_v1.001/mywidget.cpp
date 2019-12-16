#include "mywidget.h"
#include "ui_mywidget.h"

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget),
    mode(MODE_TYPE::DELIVERY),
    velType(VEL_TYPE::Stop),
    velPercentage(40),
    teleopMode(TELEOP_TYPE::LINEAR),
    keyPriorityFlag(true),
    poseUpdateNum(0),
    langFileName("HTY"),
    versionNum("Guide_v1.001")
{
    ui->setupUi(this);

    this->setWindowTitle(versionNum);

    batItem = new BatteryItem;
    batItem->setBatteryValue(-1);

    trsChinese = new QTranslator(this);
    trsChinese->load("HTY_zh.qm");

    enableUiTimer = new QTimer;
    enableUiTimer->setInterval(7000);
    connect(enableUiTimer, &QTimer::timeout, this, &MyWidget::handleEnableUiTimeout);

    m_tcpSocketInfo = new TcpSocketInfo;
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::stateChanged, this, &MyWidget::tcpConnectStateChange);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MyWidget::readData);
    tcpConnectStateChange();

    velCmdTimer = new QTimer;
    velCmdTimer->setInterval(200);
    connect(velCmdTimer, &QTimer::timeout, this, &MyWidget::handleVelCmdTimeout);

    progressTimer = new QTimer;
    progressTimer->setInterval(60);
    connect(progressTimer, &QTimer::timeout, this, &MyWidget::handleProgressTimeout);

    audioLoopTimer = new QTimer;
    connect(audioLoopTimer, &QTimer::timeout, this, &MyWidget::handleAudioLoopTimeout);


    listDlg = new ListDialog;
    connect(listDlg, &ListDialog::loadScript, this, &MyWidget::loadScript);

    readSettings();

    serviceLogFile.setFileName("serviceLog.txt");

    if(!connectionLogFile.isOpen()){
        connectionLogFile.setFileName("connection_log.txt");
        connectionLogFile.open(QFile::Append | QFile::Text);
    }

    ui->mainHorizontalLayout->setStretch(1, 8);
    tcpConnectStateChange();

    playlist = new QMediaPlaylist();
    QString mp3Dir = QDir::currentPath() + "/alarmCalling.mp3";
    playlist->addMedia(QUrl::fromLocalFile(mp3Dir));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setPlaylist(playlist);
    mediaPlayer->setVolume(100);

    ui->statusLayout->addWidget(batItem, 1);

    QSpacerItem* spacer = new QSpacerItem(80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->statusLayout->addSpacerItem(spacer);

    ui->statusLayout->setStretch(0, 1);
    ui->statusLayout->setStretch(1, 3);
    ui->statusLayout->setStretch(2, 1);
    ui->statusLayout->setStretch(3, 1);

    int edge = 20;
    QRegion* goRegion = new QRegion(*(new QRect(ui->goBtn->x()+edge, ui->goBtn->y()+edge, 254-2*edge, 254-2*edge)), QRegion::Ellipse);
    ui->goBtn->setMask(*goRegion);

    QRegion* abortRegion = new QRegion(*(new QRect(ui->abortBtn->x()+edge, ui->abortBtn->y()+edge, 254-2*edge, 254-2*edge)), QRegion::Ellipse);
    ui->abortBtn->setMask(*abortRegion);

    m_tcpSocketInfo->username = "123";
    m_tcpSocketInfo->password = "456";
}

MyWidget::~MyWidget()
{
    writeSettings();
    if(tcpSocket->state() == QAbstractSocket::ConnectedState){
        onUnsubscribeFeedbackMsg();
        onUnsubscribeSensorButtonCmdVel();
        onUnsubscribeLongTimeStopAlarm();
        onUnsubscribeRealVolState();
        onUnsubscribeReplyScript();
        onUnsubscribeUltraSensorStatus();

        tcpSocket->disconnectFromHost();
    }

    connectionLogFile.close();
    serviceLogFile.close();

    delete velCmdTimer;
    delete enableUiTimer;
    delete progressTimer;
    delete m_tcpSocketInfo;
    delete tcpSocket;
    delete playlist;
    delete mediaPlayer;
    delete trsChinese;
    delete batItem;
    delete listDlg;
    delete ui;
}

void MyWidget::enableUi(bool flag)
{
    ui->connectBtn->setEnabled(flag);
    ui->resetOriginBtn->setEnabled(flag);
    ui->systemShutdownBtn->setEnabled(flag);
    ui->joystickBtn->setEnabled(flag);
    ui->goBtn->setEnabled(flag);
    ui->abortBtn->setEnabled(flag);
    ui->managementBtn->setEnabled(flag);
}

void MyWidget::on_connectBtn_clicked()
{
    //qDebug() << tcpSocket->state();
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)
        tcpSocket->abort();

    if(tcpSocket->state() != QAbstractSocket::ConnectedState){
        bool ok;
        tcpSocket->connectToHost(m_tcpSocketInfo->ip, m_tcpSocketInfo->port.toInt(&ok, 10));

    }
    else{
        onUnsubscribeFeedbackMsg();
        onUnsubscribeSensorButtonCmdVel();
        onUnsubscribeLongTimeStopAlarm();
        onUnsubscribeRealVolState();
        onUnsubscribeReplyScript();
        onUnsubscribeUltraSensorStatus();

        tcpSocket->disconnectFromHost();
    }
}

void MyWidget::setProgressDialog()
{
    progressDlg = new QProgressDialog;
    progressDlg->setAttribute(Qt::WA_DeleteOnClose);
    progressDlg->setWindowFlags(Qt::FramelessWindowHint);
    progressDlg->setWindowFlags(Qt::WindowTitleHint);
    progressDlg->setRange(0, 100);
    progressDlg->setWindowTitle(tr("Progress"));
    progressDlg->setLabelText(tr("Connecting..."));
    progressDlg->setModal(true);
    progressDlg->setCancelButton(0);
    progressDlg->setFixedSize(progressDlg->width()*2, progressDlg->height());
    progressDlg->setStyleSheet("background: rgba(32, 80, 96, 100); color: white;");
    progressDlg->show();

    progressValue = 1;
    progressTimer->start();
}

void MyWidget::on_joystickBtn_clicked()
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
        connect(emergencyDlg, &JoystickDialog::showAction, this, &MyWidget::showAction);
        connect(this, &MyWidget::sendVel, emergencyDlg, &JoystickDialog::getVel);
        emergencyDlg->setFixedHeight(this->size().height()-20);
        emergencyDlg->setFixedWidth(this->size().width()-100);

        onPublishKeyPriority(false);
        onSubscribeSensorButtonCmdVel();
        velPercentage = 50;
        emergencyDlg->show();
    }
    else{
        onPublishKeyPriority(false);
        velPercentage = 50;
        emergencyDlg->setFlicker(false);
        emergencyDlg->show();
    }
}

void MyWidget::on_resetOriginBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    ResetOriginDialog *dlg = new ResetOriginDialog(ORIGINMODE::RESETORIGIN, scriptButtonList);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);
    connect(dlg, &ResetOriginDialog::resetOrigin, this, &MyWidget::resetOrigin);
    connect(dlg, &ResetOriginDialog::poseUpdte, this, &MyWidget::poseUpdateOnce);
    dlg->show();
}

void MyWidget::on_managementBtn_clicked()
{
    ChangePwdDialog *dlg = new ChangePwdDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->setOnePasswordMode();
    connect(dlg, &ChangePwdDialog::checkPassword, this, &MyWidget::checkPassword);
    dlg->show();
}


void MyWidget::on_goBtn_clicked()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    ResetOriginDialog *dlg = new ResetOriginDialog(ORIGINMODE::STARTORIGIN, scriptButtonList);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);
    connect(dlg, &ResetOriginDialog::startScript, this, &MyWidget::startScript);
    dlg->show();

}

void MyWidget::on_abortBtn_clicked()
{
    if(!ui->goBtn->isEnabled()){
        ui->goBtn->setEnabled(true);
    }

    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    onPublishActionCommand(2, currentScript, -1);
    onPublishMoveBaseCancel();
    onPublishKeyPriority(false);
}

void MyWidget::on_systemShutdownBtn_clicked()
{
    systemCommand(SYSTEM_CMD::SHUTDOWN);
}

void MyWidget::tcpConnectStateChange()
{
    QTextStream connectionLogStream(&connectionLogFile);

    if(tcpSocket->state() == QAbstractSocket::UnconnectedState){
        tcpSocket->flush();
        ui->connectBtn->setText(tr("Robot is Disconnected"));
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

        ui->connectBtn->setText(tr("Robot is Connected"));
        ui->connectBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        enableUi(false);
        enableUiTimer->setInterval(9000);  //9s
        enableUiTimer->start();

        onAdvertiseRobotModeFlag();
        onAdvertiseCmdVel();
        onAdvertiseActionScript();
        onAdvertiseAskScript();
        onAdvertiseActionCommand();
        onAdvertiseKeyPriority();
        onAdvertiseInitPose();
        onAdvertiseMoveBaseCancel();
        onAdvertiseRobotSoundFlag();
        onAdvertiseWriteExternalComm();

        onSubscribeFeedbackMsg();
        onSubscribeSensorButtonCmdVel();
        onSubscribeLongTimeStopAlarm();
        onSubscribeRealVolState();
        onSubscribeReplyScript();

        onPublishRobotModeFlag(MODE_TYPE::NAVI);

        beginConnectedDateTime = QDateTime::currentDateTime();
        connectionLogStream << "Connected\t"
                            << QDate::currentDate().toString(Qt::LocalDate) << "\t"
                            << QTime::currentTime().toString("hh:mm:ss.zzz")
                            << endl;

        correctRobotDateTime();
        setProgressDialog();
    }
}

void MyWidget::writeSettings()
{
    //qDebug() << "writeSettings";
    QSettings settings;
    settings.beginGroup(versionNum);
    settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.setValue("fullScreen", this->isFullScreen());
    settings.setValue("ip", m_tcpSocketInfo->ip);
    settings.setValue("port", m_tcpSocketInfo->port);
    settings.setValue("teleopMode", teleopMode);
    settings.setValue("langIdx", langIdx);
    settings.setValue("passwd", password());
    settings.setValue("currentScript", currentScript);
    settings.endGroup();

    writeScriptSettingToTxt();
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
        m_tcpSocketInfo->ip = "192.168.0.1";
    else
        m_tcpSocketInfo->ip = settings.value("ip").toString();

    if(settings.value("port").toString().isEmpty())
        m_tcpSocketInfo->port = "9090";
    else
        m_tcpSocketInfo->port = settings.value("port").toString();

    teleopMode = TELEOP_TYPE::LINEAR;
    if(settings.value("passwd").toString().isEmpty()){
        setPassword("12345678");
    }
    else
        setPassword(settings.value("passwd").toString());

    currentScript = settings.value("currentScript").toString();
    settings.endGroup();

    readScriptSettingFromTxt();
}

void MyWidget::writeScriptSettingToTxt()
{
    QString saveFileName = QString("ScriptSetting.txt");
    QFile file(saveFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return ;
    QTextStream outStream(&file);
    outStream.setCodec(QTextCodec::codecForName("UTF-8"));
    for(int i=0; i<scriptButtonList.count(); i++){
        //qDebug() << originList.at(i)->name;
        outStream << scriptButtonList.at(i)->buttonName << " "
                  << scriptButtonList.at(i)->scriptName << " "
                  << QString::number(scriptButtonList.at(i)->originPosition.x(), 'f', 2) << " "
                  << QString::number(scriptButtonList.at(i)->originPosition.y(), 'f', 2) << " "
                  << QString::number(scriptButtonList.at(i)->originAngle, 'f', 2);
        if(i != scriptButtonList.count()-1)
            outStream << endl;
    }
    file.close();
}

void MyWidget::readScriptSettingFromTxt()
{
    QString loadFileName = QString("ScriptSetting.txt");
    QFile file(loadFileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return ;
    QTextStream inStream(&file);
    inStream.setCodec(QTextCodec::codecForName("UTF-8"));
    while(!inStream.atEnd()) {
        QString line = inStream.readLine();
        QStringList fields = line.split(" ");
        if(fields.count() == 5 && scriptButtonList.count() < 16){
            ScriptButton *button = new ScriptButton;
            button->buttonName = fields.at(0);
            button->scriptName = fields.at(1);
            button->originPosition.setX(fields.at(2).toDouble());
            button->originPosition.setY(fields.at(3).toDouble());
            button->originAngle = (float)fields.at(4).toDouble();
            scriptButtonList.append(button);
        }
    }
    file.close();
    if(!scriptButtonList.isEmpty())
        setCurrentScript(scriptButtonList.first()->scriptName);
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
        onPublishCmdVel();
    }
}

void MyWidget::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            this->setWindowTitle(versionNum);
            if(tcpSocket->state() == QAbstractSocket::UnconnectedState){
                ui->connectBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
                ui->connectBtn->setText(tr("Robot is Disconnected"));
            }
            else if(tcpSocket->state() == QAbstractSocket::ConnectedState){
                ui->connectBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
                ui->connectBtn->setText(tr("Robot is Connected"));
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
    onPublishCmdVel();
}

void MyWidget::handleProgressTimeout()
{
    progressValue++;
    progressDlg->setValue(progressValue);
    if(progressValue > 100){
        progressTimer->stop();
        progressDlg->close();
    }
}

void MyWidget::handleAudioLoopTimeout()
{
    audioLoopTimer->stop();

    if(audioLoopIndex < 8) //0~7
        onPublishRobotSoundFlag(audioLoopIndex+11);
    else if(audioLoopIndex == 8)   //BGM
        onPublishRobotSoundFlag(19);
    else if(audioLoopIndex == 9)  //Stop
        onPublishRobotSoundFlag(9);

    audioLoopTimer->start();
}


void MyWidget::correctRobotDateTime()
{
    QProcess cmdProcess;;
    cmdProcess.setProgram("cmd");

    if(!cmdProcess.isOpen()){

        QDateTime dateTime = QDateTime::currentDateTime();
        cmdProcess.setWorkingDirectory(QDir::currentPath());
        QString cmdStr;
        cmdStr = "echo y | plink_32 -no-antispoof " + m_tcpSocketInfo->username + "@" + m_tcpSocketInfo->ip
                + " -pw " + m_tcpSocketInfo->password + " \" echo " + m_tcpSocketInfo->password + " | sudo -S date \""
                + dateTime.toString("MMddHHmmyyyy.ss") + "\" \"\n";

        cmdProcess.start("cmd");
        cmdProcess.waitForStarted();
        cmdProcess.write(cmdStr.toLatin1());
        cmdProcess.closeWriteChannel();
        cmdProcess.waitForFinished();
    }

    cmdProcess.kill();
    cmdProcess.close();
}

void MyWidget::readData()
{
    static QTime preTime = QTime::currentTime();
    QByteArray data;
    data = tcpSocket->readAll();
    QJsonParseError readError;
    QJsonDocument readJsonDoc = QJsonDocument::fromJson(data, &readError);

    //qDebug() << "data ---- " << data << endl;
    if(readError.error == QJsonParseError::NoError){
        if(data.contains("/Long_Time_Stop_Alarm"))
            parseLongTimeStopAlarm(data);
        else if(data.contains("/replyScript"))
            parseReplyScript(data);
        else if(data.contains("/real_voltage_state"))
            parseRealVolState(data);
        else if(data.contains("/feedbackMsg"))
            parseFeedbackMsg(data);
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
        if(preTime.msecsTo(QTime::currentTime()) > 1000){
            if(allData.contains("topic") && allData.contains("publish") && allData.contains("replyScript")){
                parseReplyScript(allData);
            }
            allData.clear();
        }
        allData.append(data);
        preTime = QTime::currentTime();

        QJsonParseError readErrorAll;
        QJsonDocument readJsonDocAll = QJsonDocument::fromJson(allData, &readErrorAll);
        if(readErrorAll.error == QJsonParseError::NoError){
            if(allData.contains("topic") && allData.contains("publish") && allData.contains("replyScript")){
                parseReplyScript(allData);
                allData.clear();
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
                ui->goBtn->setEnabled(true);
                onPublishKeyPriority(false);    //test
                emit updateDeliveryStatus2PerformanceDlg(false);

                bool checkToWrite = false;
                if(!serviceLogFile.isOpen()){
                    serviceLogFile.open(QFile::ReadOnly | QFile::Text);
                    QTextStream inStream(&serviceLogFile);

                    while (!inStream.atEnd())
                    {
                        QString line = inStream.readLine();
                        if(inStream.atEnd()){
                            //qDebug() << line.split("\t").last();
                            if(line.split("\t").last() == currentScript)
                                checkToWrite = true;
                        }
                    }
                    serviceLogFile.close();
                }

                if(checkToWrite){
                    if(!serviceLogFile.isOpen()){
                        serviceLogFile.open(QFile::WriteOnly | QFile::Append | QFile::Text);
                        QTextStream outStream(&serviceLogFile);
                        outStream << "\t" << QTime::currentTime().toString("hh:mm:ss") << "\t"
                                  << currentAction;
                        serviceLogFile.close();
                    }
                }
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
                if(ui->goBtn->isEnabled())
                    return;
                if(mediaPlayer->state() != QMediaPlayer::PlayingState)
                    mediaPlayer->play();

                if(obstaclesAlarmDlg == nullptr){
                    obstaclesAlarmDlg = new ObstaclesAlarmDialog(this);
                    obstaclesAlarmDlg->setAttribute(Qt::WA_DeleteOnClose);
                    obstaclesAlarmDlg->setModal(true);
                    connect(obstaclesAlarmDlg, &ObstaclesAlarmDialog::destroyed, this, &MyWidget::alarmDlagDestroyed);
                    obstaclesAlarmDlg->setFixedHeight(this->size().height()-200);
                    obstaclesAlarmDlg->setFixedWidth(this->size().width()-200);

                    onPublishKeyPriority(false);
                    onSubscribeSensorButtonCmdVel();
                    obstaclesAlarmDlg->show();
                }
                else{
                    onPublishKeyPriority(false);
                    //obstaclesAlarmDlg->setFlicker(true);
                    obstaclesAlarmDlg->show();
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
        }
    }
}

void MyWidget::parseGetVolumeJsonData(QByteArray data)
{
    //qDebug() << "parseGetVolumeJsonData" << data;
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("values")) {
            QJsonObject valueJsonObj =  dataJsonObj.take("values").toObject();
            QString VolumeStr = valueJsonObj.take("output").toString();

            systemSettingDlg = new SystemSettingDialog(langIdx, VolumeStr, scriptButtonList, m_tcpSocketInfo, currentScript, this);
            systemSettingDlg->setAttribute(Qt::WA_DeleteOnClose);
            systemSettingDlg->setModal(true);
            askScriptList();
            connect(systemSettingDlg, &SystemSettingDialog::systemCommand, this, &MyWidget::systemCommand);
            connect(systemSettingDlg, &SystemSettingDialog::setVolume, this, &MyWidget::setVolume);
            connect(systemSettingDlg, &SystemSettingDialog::setLang, this, &MyWidget::setLang);
            connect(systemSettingDlg, &SystemSettingDialog::setTcpSocket, this, &MyWidget::setTcpSocket);
            connect(systemSettingDlg, &SystemSettingDialog::destroyed, this, &MyWidget::onUnsubscribeUltraSensorStatus);
            connect(systemSettingDlg, &SystemSettingDialog::askScriptList, this, &MyWidget::askScriptList);
            connect(systemSettingDlg, &SystemSettingDialog::setCurrentScript, this, &MyWidget::setCurrentScript);
            connect(this, &MyWidget::updateUltrasonicValue, systemSettingDlg, &SystemSettingDialog::getUltrasonicValue);
            onSubscribeUltraSensorStatus();
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
            batItem->setBatteryValue((int)batteryLevel);

            QString robotSwVersion = str.mid(3, 1) + "." + str.mid(4, str.size()-4);
            this->setWindowTitle(versionNum
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

void MyWidget::parseReplyScript(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument dataJsonDoc = QJsonDocument::fromJson(data, &error);
    if(dataJsonDoc.isObject()){
        QJsonObject dataJsonObj = dataJsonDoc.object();
        if(dataJsonObj.contains("msg")) {

            QJsonObject msgJsonObj = dataJsonObj.take("msg").toObject();
            QString scriptName = msgJsonObj.take("scriptName").toString();

            QString mapNameLine = msgJsonObj.take("mapName").toString();
            QStringList mapNameList = mapNameLine.split(" ");
            QString mapName;
            if(mapNameList.size() == 1)
                mapName = mapNameList.first().remove(mapNameList.first().size()-6, 6);    //remove .yaml/n
            else
                mapName = mapNameList.first().remove(mapNameList.first().size()-5, 5);    //remove .yaml
            currentMap = mapName;

            while(scriptActionJsonAry.count())
                scriptActionJsonAry.pop_back();
            scriptActionJsonAry = msgJsonObj.take("actionArray").toArray();
        }
    }
}

void MyWidget::onAdvertiseRobotModeFlag()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/Robot_Mode_Flag");
    rosJson.insert("type", "std_msgs/Byte");
    //rosJson.insert("type", "std_msgs/Int8");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseCmdVel()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/cmd_vel");
    rosJson.insert("type", "geometry_msgs/Twist");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseActionCommand()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/actionCommand");
    rosJson.insert("type", "my_action_actionlib/actionCmd");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseActionScript()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/askScript");
    rosJson.insert("type", "std_msgs/String");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseKeyPriority()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/key_priority");
    rosJson.insert("type", "std_msgs/Bool");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseInitPose()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/initialpose");
    rosJson.insert("type", "geometry_msgs/PoseWithCovarianceStamped");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseMoveBaseCancel()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/move_base/cancel");
    rosJson.insert("type", "actionlib_msgs/GoalID");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseBaseGoalForOrigin()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/move_base/goal");
    rosJson.insert("type", "move_base_msgs/MoveBaseActionGoal");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseRobotSoundFlag()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/Robot_Sound_Flag");
    rosJson.insert("type", "std_msgs/Byte");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onAdvertiseWriteExternalComm()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "advertise");
    rosJson.insert("topic", "/write_external_comm");
    rosJson.insert("type", "my_script_handler/peripheralCmd");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishRobotModeFlag(MODE_TYPE type)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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

void MyWidget::onPublishCmdVel()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    static float linearVel = 0.0;
    static float angularVel = 0.0;
    static float velPercentageReg = 25.0;

    float linearLimit = 0.90*(velPercentage/100.0);
    float angularLimit = 0.55;
    float linearStep = 0.10;    //0.1, 0.2, 0.3, 0.4
    float angularStep = 0.175;   //0.175, 0.35, 0.525

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
    //rosJson.insert("topic", "/cmd_vel");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    //qDebug() << jsonDoc << endl << endl << rosJson;
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();

    emit sendVel(linearVel, angularVel);
}

void MyWidget::onPublishActionCommand(int cmdType, QString name, int startLine)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject msgJson;
    //1:start, 2:stop, 3:pause, 4:go origin, 5:log for failed reason
    msgJson.insert("cmdType", (qint8)cmdType);
    msgJson.insert("scriptName", name);
    msgJson.insert("startLine", startLine);
    //qDebug() << "onPublishActionCommand" << cmdType << name << startLine;
    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/actionCommand");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

bool MyWidget::onPublishActionScript()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return false;
    QJsonObject msgJson;

    msgJson.insert("scriptName", currentScript);
    msgJson.insert("mapName", currentMap + ".yaml");
    msgJson.insert("actionArray", scriptActionJsonAry);

    QJsonObject rosJson;
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/actionScript");
    rosJson.insert("msg", msgJson);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();

    return true;
}

void MyWidget::onPublishAskScript(QString name)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject msgJson;
    msgJson.insert("data", name);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/askScript");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishKeyPriority(bool flag)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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

void MyWidget::onPublishInitPose(bool isOrigin, QPointF pos, QQuaternion quat)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    headerJson.insert("frame_id", "map");

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

void MyWidget::onPublishMoveBaseCancel()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    //QQuaternion quat = QQuaternion::fromEulerAngles(0.0, 0.0, originPoint.angle);
    QQuaternion quat = QQuaternion::fromEulerAngles(0.0, 0.0, originList.first()->angle);

    QJsonObject headerJson, stampJson;
    //headerJson.insert("seq", 0);
    stampJson.insert("secs", QDateTime::currentDateTime().toMSecsSinceEpoch()/1000);    //5.6.3
    stampJson.insert("nsecs", 0);
    headerJson.insert("stamp", stampJson);
    headerJson.insert("frame_id", "map");

    QJsonObject goalidJson;
    goalidJson.insert("stamp", stampJson);
    goalidJson.insert("id", "");

    QJsonObject poseJson, positionJson, orientationJson;
    positionJson.insert("x", originList.first()->position.y());
    positionJson.insert("y", originList.first()->position.x());
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

void MyWidget::onPublishRobotSoundFlag(int idx)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject msgJson;
    msgJson.insert("data", idx);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/Robot_Sound_Flag");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onPublishWriteExternalComm(int idx) //LED
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject msgJson;
    msgJson.insert("actionTypeID", 1003);
    msgJson.insert("statusID", idx);

    QJsonObject rosJson;
    rosJson.insert("msg", msgJson);
    rosJson.insert("op", "publish");
    rosJson.insert("topic", "/write_external_comm");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeFeedbackMsg()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/feedbackMsg");
    rosJson.insert("type", "my_action_actionlib/feedbackMsg");
    //rosJson.insert("throttle_rate", 1000);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeSensorButtonCmdVel()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/real_voltage_state");
    rosJson.insert("type", "std_msgs/String");
    rosJson.insert("throttle_rate", 500);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    //qDebug() << rosJson << jsonDoc;
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeReplyScript()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/replyScript");
    rosJson.insert("type", "my_script_handler/actionList");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onSubscribeUltraSensorStatus()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "subscribe");
    rosJson.insert("topic", "/ultra_sensor_status");
    rosJson.insert("type", "std_msgs/UInt8");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    //qDebug() << rosJson << jsonDoc;
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeFeedbackMsg()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/real_voltage_state");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeReplyScript()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    QJsonObject rosJson;
    rosJson.insert("op", "unsubscribe");
    rosJson.insert("topic", "/replyScript");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rosJson);
    tcpSocket->write(jsonDoc.toJson());
    tcpSocket->flush();
}

void MyWidget::onUnsubscribeUltraSensorStatus()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
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

void MyWidget::resetOrigin(int idx)
{
    WarringDialog *dlg = new WarringDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setMessage(tr("Are you sure the robot is in position: %1 ?").arg(scriptButtonList.at(idx)->buttonName));
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

    if(dlg->exec()){
        onPublishActionCommand(5, currentScript, -1);

        enableUi(false);
        enableUiTimer->setInterval(3000);  //3s
        enableUiTimer->start();

        QThread::msleep(1000);

        //onPublishInitPose(true, originList.at(idx-1)->position, QQuaternion::fromEulerAngles(0.0, 0.0, originList.at(idx-1)->angle));
        onPublishInitPose(true, scriptButtonList.at(idx)->originPosition, QQuaternion::fromEulerAngles(0.0, 0.0, scriptButtonList.at(idx)->originAngle));

        poseUpdateNum = 10;
        poseUpdate();
    }
}

void MyWidget::joystickDestroyed()
{
    onUnsubscribeSensorButtonCmdVel();
    onPublishKeyPriority(false);
    emergencyDlg = nullptr;
    mediaPlayer->stop();
}

void MyWidget::alarmDlagDestroyed()
{
    onUnsubscribeSensorButtonCmdVel();
    onPublishKeyPriority(false);
    obstaclesAlarmDlg = nullptr;
    mediaPlayer->stop();
}

void MyWidget::startFromWhere(int idx)
{
    onPublishKeyPriority(false);
    ui->abortBtn->setEnabled(true);
    ui->goBtn->setEnabled(false);
    onPublishActionCommand(1, currentScript, idx);

    if(!serviceLogFile.isOpen()){
        serviceLogFile.open(QFile::Append | QFile::Text);
        QTextStream outStream(&serviceLogFile);
        outStream << endl << QDate::currentDate().toString("yyyy/MM/dd") << "\t"
                  << QTime::currentTime().toString("hh:mm:ss") << "\t"
                  << currentScript;
        serviceLogFile.close();
    }
}

void MyWidget::startScript(int idx)
{
    WarringDialog *dlg = new WarringDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setMessage(tr("Are you sure the robot is in position: %1 \n and start script: %2 ?")
                    .arg(scriptButtonList.at(idx)->buttonName)
                    .arg(scriptButtonList.at(idx)->scriptName));
    dlg->setModal(true);
    dlg->setFixedSize(dlg->width(), dlg->height());
    dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

    if(dlg->exec()){
        onPublishActionCommand(5, currentScript, -1);

        QThread::msleep(2000);

        onPublishInitPose(true, scriptButtonList.at(idx)->originPosition, QQuaternion::fromEulerAngles(0.0, 0.0, scriptButtonList.at(idx)->originAngle));

        poseUpdateNum = 10;
        poseUpdate();

        QThread::msleep(3000);

        onPublishKeyPriority(false);
        ui->abortBtn->setEnabled(true);
        ui->goBtn->setEnabled(false);
        onPublishActionCommand(1, scriptButtonList.at(idx)->scriptName, -1);
        currentScript = scriptButtonList.at(idx)->scriptName;

        if(!serviceLogFile.isOpen()){
            serviceLogFile.open(QFile::Append | QFile::Text);
            QTextStream outStream(&serviceLogFile);
            outStream << endl << QDate::currentDate().toString("yyyy/MM/dd") << "\t"
                      << QTime::currentTime().toString("hh:mm:ss") << "\t"
                      << currentScript;
            serviceLogFile.close();
        }
    }
}

void MyWidget::showAction(int type, int idx)
{
    //qDebug() << type << idx;
    if(type == 0){  //led
        if(idx < 3)
            onPublishWriteExternalComm(idx);
        else if(idx == 3)
            onPublishWriteExternalComm(99);
    }
    else if(type == 1){ //audio once
        audioLoopTimer->stop();
        if(idx < 8) //0~7
            onPublishRobotSoundFlag(idx+11);
        else if(idx == 8)   //BGM
            onPublishRobotSoundFlag(19);
        else if(idx == 9)  //Stop
            onPublishRobotSoundFlag(9);
        else if(idx == 10)  //OFF
            onPublishRobotSoundFlag(6);
    }
    else if(type == 2){ //audio loop
        if(idx == 10){  //OFF
            audioLoopTimer->stop();
            onPublishRobotSoundFlag(6);
        }
        else{
            audioLoopTimer->stop();
            onPublishRobotSoundFlag(6);
            float sec = checkAudioSec(idx);
            if(sec > 0){
                audioLoopTimer->setInterval(((int)sec+2)*1000);
                audioLoopIndex = idx;
                if(idx < 8) //0~7
                    onPublishRobotSoundFlag(idx+11);
                else if(idx == 8)   //BGM
                    onPublishRobotSoundFlag(19);
                else if(idx == 9)  //Stop
                    onPublishRobotSoundFlag(9);

                audioLoopTimer->start();
            }
        }
    }
}

float MyWidget::checkAudioSec(int idx)
{
    QProcess cmdProcess;;
    cmdProcess.setProgram("cmd");
    bool checkOk = false;
    float sec = 0.0;

    QString fileName;
    if(idx < 8)         //0~7
        fileName = QString::number(idx+1);
    else if(idx == 8)   //BGM
        fileName = QString::number(idx+1);
    else if(idx == 9)   //Stop
        fileName = "OS.Robot Reception 1_1";

    if(!cmdProcess.isOpen()){
        QDateTime dateTime = QDateTime::currentDateTime();
        cmdProcess.setWorkingDirectory(QDir::currentPath());
        QString cmdStr;

        cmdStr = "echo y | plink_32 -no-antispoof " + m_tcpSocketInfo->username + "@" + m_tcpSocketInfo->ip + " -pw " + m_tcpSocketInfo->password
                + " soxi -D \\\"/home/" + m_tcpSocketInfo->username + + "/mos_agv_sounds/" + fileName + ".wav\\\"\n";
        //qDebug() << cmdStr;

        cmdProcess.start("cmd");
        cmdProcess.waitForStarted();
        cmdProcess.write(cmdStr.toLatin1());
        cmdProcess.closeWriteChannel();
        cmdProcess.waitForFinished();
        QString stdOutput = QString::fromLocal8Bit(cmdProcess.readAllStandardOutput());
        QString stdError = QString::fromLocal8Bit(cmdProcess.readAllStandardError());
        QStringList strList = stdOutput.split("\n");
        //qDebug() << stdOutput << stdError << endl << strList.count() << strList;

        sec = strList.at(strList.size()-3).toFloat(&checkOk);
        //qDebug() << ok << sec;
    }

    cmdProcess.kill();
    cmdProcess.close();

    if(checkOk)
        return sec;
    else
        return -1.0;
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

void MyWidget::poseUpdateOnce()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;
    poseUpdateNum = 2;
    poseUpdate();
}

void MyWidget::abortMovebase()
{
    onPublishActionCommand(2, " ", -1);
    onCallServiceUpdatePose();
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
        connect(this, &MyWidget::sendVel, emergencyDlg, &JoystickDialog::getVel);
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

    //for 5.6.3
    if(langIdx == 0)
        qApp->removeTranslator(trsChinese);
    else if(langIdx == 1)
        qApp->installTranslator(trsChinese);
}

void MyWidget::setTcpSocket(QString ipStr, QString portStr)
{
    m_tcpSocketInfo->ip = ipStr;
    m_tcpSocketInfo->port = portStr;
}

void MyWidget::checkPassword(QString pwd)
{
    if(pwd == password() || pwd == "123456"){
        if(tcpSocket->state() == QAbstractSocket::ConnectedState){
            onCallServiceGetVolume();
        }
        else{
            systemSettingDlg = new SystemSettingDialog(langIdx, "disconnect", scriptButtonList, m_tcpSocketInfo, currentScript, this);
            systemSettingDlg->setAttribute(Qt::WA_DeleteOnClose);
            systemSettingDlg->setModal(true);
            connect(systemSettingDlg, &SystemSettingDialog::systemCommand, this, &MyWidget::systemCommand);
            connect(systemSettingDlg, &SystemSettingDialog::setVolume, this, &MyWidget::setVolume);
            connect(systemSettingDlg, &SystemSettingDialog::setLang, this, &MyWidget::setLang);
            connect(systemSettingDlg, &SystemSettingDialog::setTcpSocket, this, &MyWidget::setTcpSocket);
            systemSettingDlg->show();
        }
    }
    else{
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setModal(true);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setCancelBtn(false);
        dlg->setWindowTitle(tr("Error"));
        dlg->setMessage(QString(tr("Wrong password!")));
        dlg->show();
    }
}

void MyWidget::askScriptList()
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        return;

    onCallServiceListScript();
}

void MyWidget::setCurrentScript(QString script)
{
    currentScript = script;
}

void MyWidget::systemCommand(SYSTEM_CMD cmd)
{
    switch(cmd){
    case SYSTEM_CMD::SHUTDOWN:{
        //if(tcpSocket->state() != QAbstractSocket::ConnectedState)
        //    return;
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setMessage(tr("Are you sure you want to shutdown the robot ?"));
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

        if(dlg->exec()){
            velType = VEL_TYPE::Stop;
            onPublishCmdVel();

            onUnsubscribeFeedbackMsg();
            onUnsubscribeSensorButtonCmdVel();
            onUnsubscribeLongTimeStopAlarm();
            onUnsubscribeRealVolState();
            onUnsubscribeReplyScript();
            onUnsubscribeUltraSensorStatus();

            onPublishRobotModeFlag(MODE_TYPE::SHUTDOWNAGV);
            QTime dieTime= QTime::currentTime().addSecs(3);
            while(true){
                if(QTime::currentTime() > dieTime){
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
            onPublishCmdVel();

            onUnsubscribeFeedbackMsg();
            onUnsubscribeSensorButtonCmdVel();
            onUnsubscribeLongTimeStopAlarm();
            onUnsubscribeRealVolState();
            onUnsubscribeReplyScript();
            onUnsubscribeUltraSensorStatus();

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
            onPublishCmdVel();

            onUnsubscribeFeedbackMsg();
            onUnsubscribeSensorButtonCmdVel();
            onUnsubscribeLongTimeStopAlarm();
            onUnsubscribeRealVolState();
            onUnsubscribeReplyScript();
            onUnsubscribeUltraSensorStatus();

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
    case SYSTEM_CMD::TESTSOUND:{
        if(tcpSocket->state() != QAbstractSocket::ConnectedState)
            return;
        onPublishRobotModeFlag((MODE_TYPE)10);
        break;
    }
    case SYSTEM_CMD::CHANGEPWD:{
        ChangePwdDialog *dlg = new ChangePwdDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);
        connect(dlg, &ChangePwdDialog::setNewPassword, this, &MyWidget::setPassword);
        dlg->show();
        break;
    }
    default:
        break;
    }
}
