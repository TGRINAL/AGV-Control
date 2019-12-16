#include "systemsettingdialog.h"
#include "ui_systemsettingdialog.h"

SystemSettingDialog::SystemSettingDialog(int langIdx, QString volumeStr, QList<ScriptButton *> buttonList,
                                         TcpSocketInfo *socket, QString currentScript, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemSettingDialog)
{
    ui->setupUi(this);
    //setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    langIndex = langIdx;
    m_buttonList = buttonList;
    for(int i=0; i<buttonList.count(); i++)
        ui->originCB->addItem(buttonList.at(i)->buttonName);

    volumeStr.remove("%");
    volumeStr.remove("\n");
    //qDebug() << volumeStr;
    ui->bgmBtn->setVisible(false);
    if(volumeStr.contains("disconnect")){  //disconnect
        ui->volumeScrollBar->setVisible(false);
        ui->volumeLabel->setVisible(false);
        ui->volumeIcon->setVisible(false);
        ui->lowLabel->setVisible(false);
        ui->highLabel->setVisible(false);
        ui->testSoundBtn->setVisible(false);
        //ui->bgmBtn->setVisible(false);
        ui->soundModifyBtn->setVisible(false);
        ui->ultrasonicLabel->setVisible(false);
        ui->ultrasonicLabel_1->setVisible(false);
        ui->ultrasonicLabel_2->setVisible(false);
        ui->ultrasonicLabel_3->setVisible(false);
        ui->ultrasonicLabel_4->setVisible(false);
        ui->ultrasonicLabel_5->setVisible(false);
        ui->ultrasonicLabel_6->setVisible(false);
        ui->ultrasonicIcon->setVisible(false);
        //ui->scriptSelectionCB->setVisible(false);
        ui->scriptLineEdit->setText(buttonList.first()->scriptName);
    }
    else{   //connect
        if(volumeStr.isEmpty()){    //connect but can't get volume
            /*ui->volumeScrollBar->setVisible(false);
            ui->volumeLabel->setVisible(false);
            ui->lowLabel->setVisible(false);
            ui->highLabel->setVisible(false);
            ui->testSoundBtn->setVisible(false);*/
            //ui->bgmBtn->setVisible(false);
        }
        //else{
        bool ok;
        ui->volumeScrollBar->setValue(volumeStr.toInt(&ok, 10));

        LightWidget *m_light1 = new LightWidget(Qt::gray);
        LightWidget *m_light2 = new LightWidget(Qt::gray);
        LightWidget *m_light3 = new LightWidget(Qt::gray);
        LightWidget *m_light4 = new LightWidget(Qt::gray);
        LightWidget *m_light5 = new LightWidget(Qt::gray);
        LightWidget *m_light6 = new LightWidget(Qt::gray);

        m_lightList.append(m_light1);
        m_lightList.append(m_light2);
        m_lightList.append(m_light3);
        m_lightList.append(m_light4);
        m_lightList.append(m_light5);
        m_lightList.append(m_light6);

        for(int i=0; i<m_lightList.count(); i++){
            m_lightList.at(i)->resize(ui->ultrasonicLabel->size()/2);
            //qDebug() << m_lightList.at(i)->size() << ui->ultrasonicLabel->size();
            m_lightList.at(i)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
            ui->lightGridLayout->addWidget(m_lightList.at(i), 1, i);
        }

    }

    ui->originDsb_x->setValue(buttonList.first()->originPosition.x());
    ui->originDsb_y->setValue(buttonList.first()->originPosition.y());
    ui->originDsb_angle->setValue(buttonList.first()->originAngle);

    m_socket = socket;
    ui->ipLineEdit->setText(socket->ip);
    ui->portLineEdit->setText(socket->port);
    m_currentScript = currentScript;
    m_currentStopMin = 0;
    m_currentStopSec = 0;
    ui->portLabel->setVisible(false);
    ui->portLineEdit->setVisible(false);
    ui->abnormalRecordBtn->setVisible(false);
    this->adjustSize();
}

SystemSettingDialog::~SystemSettingDialog()
{
    emit setTcpSocket(ui->ipLineEdit->text(), ui->portLineEdit->text());
    delete ui;
}

void SystemSettingDialog::on_shutdownBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::SHUTDOWN);
}

void SystemSettingDialog::on_rebootBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::REBOOT);
}

void SystemSettingDialog::on_resetStatusBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::RESETSTATUS);
}

void SystemSettingDialog::changeEvent(QEvent *event)
{
    if(0 != event){
        switch(event->type()) {
        case QEvent::LanguageChange:{
            //qDebug() << "SystemSettingDialog::changeEvent LanguageChange";
            ui->retranslateUi(this);
            break;
        }
        default:
            break;
        }
        QDialog::changeEvent(event);
    }
}

void SystemSettingDialog::on_testSoundBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::TESTSOUND);
    ui->testSoundBtn->setEnabled(false);
    ui->volumeScrollBar->setEnabled(false);
    QTimer::singleShot(5000, this, SLOT(enableVolumeTest()));
}

void SystemSettingDialog::enableVolumeTest()
{
    ui->testSoundBtn->setEnabled(true);
    ui->volumeScrollBar->setEnabled(true);
}

void SystemSettingDialog::on_volumeScrollBar_sliderReleased()
{
    emit setVolume(ui->volumeScrollBar->value());
}

void SystemSettingDialog::on_volumeScrollBar_actionTriggered(int action)
{
    if(action == QAbstractSlider::SliderSingleStepAdd || action == QAbstractSlider::SliderSingleStepSub ||
            action == QAbstractSlider::SliderPageStepAdd || action == QAbstractSlider::SliderPageStepSub){
        emit setVolume(ui->volumeScrollBar->sliderPosition());
    }
}

void SystemSettingDialog::on_langEnBtn_clicked()
{
    this->close();
    emit setLang(0);
}

void SystemSettingDialog::on_langZhBtn_clicked()
{
    this->close();
    emit setLang(1);
}

void SystemSettingDialog::getUltrasonicValue(int value)
{
    for(int i=0; i<6; i++){
        //qDebug() << ((value & (int)pow(2, i)) / (int)pow(2, i));
        if((value & (int)pow(2, i)) / (int)pow(2, i))
            m_lightList.at(i)->setColor(Qt::green);
        else
            m_lightList.at(i)->setColor(Qt::red);
    }
}

void SystemSettingDialog::on_originDsb_x_valueChanged(double arg1)
{
    m_buttonList.at(ui->originCB->currentIndex())->originPosition.setX(ui->originDsb_x->value());
}

void SystemSettingDialog::on_originDsb_y_valueChanged(double arg1)
{
    m_buttonList.at(ui->originCB->currentIndex())->originPosition.setY(ui->originDsb_y->value());
}

void SystemSettingDialog::on_originDsb_angle_valueChanged(double arg1)
{
    m_buttonList.at(ui->originCB->currentIndex())->originAngle = ui->originDsb_angle->value();
}

void SystemSettingDialog::on_changePwdBtn_clicked()
{
    emit systemCommand(SYSTEM_CMD::CHANGEPWD);
}

void SystemSettingDialog::on_originCB_currentIndexChanged(int index)
{
    ui->originDsb_x->setValue(m_buttonList.at(index)->originPosition.x());
    ui->originDsb_y->setValue(m_buttonList.at(index)->originPosition.y());
    ui->originDsb_angle->setValue(m_buttonList.at(index)->originAngle);
    ui->scriptLineEdit->setText(m_buttonList.at(index)->scriptName);
}

void SystemSettingDialog::on_bgmBtn_clicked()
{
    //QString fileName = QFileDialog::getOpenFileName(this, "Select wav file",
    //                                                QDir::currentPath(), "Audio Files (*.wav)");
    QString fileName = QFileDialog::getOpenFileName(this, "Select wav file",
                                                    QDir::homePath() + "\\Music", "Audio Files (*.wav)");
    if(fileName.isEmpty())
        return;

    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(fileName));
    connect(player, &QMediaPlayer::durationChanged, this, [&](qint64 dur) {
        ;//qDebug() << "duration = " << dur;
    });
    delete player;

    QProcess cmdProcess;;
    cmdProcess.setProgram("cmd");

    if(!cmdProcess.isOpen()){
        cmdProcess.setWorkingDirectory(QDir::currentPath());

        //1. copy to background.wav
        QString cmdStr = "copy " + fileName + " " + QDir::currentPath() + "/background.wav\n";
        cmdStr.replace("/", "\\");
        //qDebug() << cmdStr;
        cmdProcess.start("cmd");
        cmdProcess.waitForStarted();
        cmdProcess.write(cmdStr.toLatin1());
        cmdProcess.closeWriteChannel();
        cmdProcess.waitForFinished();
        //QString stdOutput = QString::fromLocal8Bit(cmdProcess.readAllStandardOutput());
        //QString stdError = QString::fromLocal8Bit(cmdProcess.readAllStandardError());
        //qDebug() << stdOutput;

        //2. transfer background.wav to robot
        cmdStr = "echo y | pscp -pw " + m_socket->password + " " + QDir::currentPath() + "/background.wav " + m_socket->username + "@"
                + m_socket->ip + ":/home/" + m_socket->username + "/mos_agv_sounds" + "\n";
        //qDebug() << cmdStr;
        cmdProcess.start("cmd");
        cmdProcess.waitForStarted();
        cmdProcess.write(cmdStr.toLatin1());
        cmdProcess.closeWriteChannel();
        cmdProcess.waitForFinished();
        QString stdOutput = QString::fromLocal8Bit(cmdProcess.readAllStandardOutput());
        QString stdError = QString::fromLocal8Bit(cmdProcess.readAllStandardError());

        //qDebug() << stdOutput << stdOutput.contains("100%");
        //qDebug() << stdError;

        WarringDialog *dlg = new WarringDialog(this);
        dlg->setModal(true);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setCancelBtn(false);

        if(stdOutput.contains("100%")){
            dlg->setWindowTitle(tr("Ok"));
            dlg->setMessage(QString(tr("Already change background music!")));
            dlg->show();
        }
        else if(stdError.contains("Connection refused")){   //not install ssh on ubuntu
            dlg->setWindowTitle(tr("Critical"));
            dlg->setMessage(QString(tr("Receiving software is not installed on the robot!\nPlease contact the service staff")));
            dlg->show();
        }
        else if(stdError.contains("Access denied")){
            dlg->setWindowTitle(tr("Critical"));
            dlg->setMessage(QString(tr("Username or password wrong!\nPlease contact the service staff")));
            dlg->show();
        }
        else if(stdError.contains("'pscp'")){
            dlg->setWindowTitle(tr("Error"));
            dlg->setMessage(QString(tr("pscp.exe not exist on application directory!\nPlease contact the service staff")));
            dlg->show();
        }

        //3. delete background.wav
        cmdStr = "del " + QDir::currentPath() + "/background.wav\n";
        cmdStr.replace("/", "\\");
        //qDebug() << cmdStr;
        cmdProcess.start("cmd");
        cmdProcess.waitForStarted();
        cmdProcess.write(cmdStr.toLatin1());
        cmdProcess.closeWriteChannel();
        cmdProcess.waitForFinished();
    }

    cmdProcess.kill();
    cmdProcess.close();
}

void SystemSettingDialog::on_serviceRecordBtn_clicked()
{
    RecordDialog *dlg = new RecordDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);
    //dlg->move(this->size().width()/2, this->size().height()/2);
    dlg->show();
}

void SystemSettingDialog::on_closeBtn_clicked()
{
    this->close();
}

void SystemSettingDialog::on_soundModifyBtn_clicked()
{
    QStringList list;
    list.append(m_socket->ip);
    list.append(m_socket->port);
    list.append(m_socket->username);
    list.append(m_socket->password);

    SoundModifyDialog *dlg = new SoundModifyDialog(list, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);
    dlg->setFixedHeight(dlg->height());
    dlg->setFixedWidth(dlg->width());
    dlg->show();
}

