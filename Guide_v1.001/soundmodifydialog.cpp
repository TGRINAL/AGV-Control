/***********************************************************************************
* Copyright ©2019 TECO Electric & Machinery Co., Ltd.                              *
*                                                                                  *
* The information contained herein is confidential property of TECO.	           *
* All rights reserved. Reproduction, adaptation, or translation without		       *
* the express written consent of TECO is prohibited, except as	                   *
* allowed under the copyright laws and LGPL 2.1 terms.                             *
***********************************************************************************/
/*
* @file            soundmodifydialog.cpp
* @author          TECO Group Research Institute   <saservice@teco.com.tw>
* @date            25 Dec 2019
* @code Version    1.1
*/
			
#include "soundmodifydialog.h"
#include "ui_soundmodifydialog.h"

SoundModifyDialog::SoundModifyDialog(QStringList list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoundModifyDialog)
{
    ui->setupUi(this);
    paraList = list;

    cmdProcess = new QProcess;
    cmdProcess->setProgram("cmd");
    connect(cmdProcess, &QProcess::readyRead, this, &SoundModifyDialog::readCmdData);

    cmdProcess->setWorkingDirectory(QDir::currentPath());

    progressDlg = new QProgressDialog;
    connect(progressDlg, &QProgressDialog::canceled, this, &SoundModifyDialog::cancelTransfer);
    progressDlg->setWindowFlags(Qt::FramelessWindowHint);
    progressDlg->setWindowFlags(Qt::WindowTitleHint);
    progressDlg->setRange(0, 100);
    progressDlg->setWindowTitle(tr("Progress"));
    progressDlg->setLabelText(tr("Transfer..."));
    progressDlg->setModal(true);
    progressDlg->setWindowModality(Qt::ApplicationModal);
    //progressDlg->setCancelButton(0);
    progressDlg->setFixedSize(progressDlg->width()*2, progressDlg->height());
    progressDlg->setStyleSheet("background: rgba(32, 80, 96, 100); color: white;");
    progressDlg->setValue(-1);
    progressDlg->close();

    warringDlg = new WarringDialog(this);
    warringDlg->setModal(true);
    warringDlg->setCancelBtn(false);
}

SoundModifyDialog::~SoundModifyDialog()
{
    delete cmdProcess;
    delete progressDlg;
    delete warringDlg;
    delete ui;
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::readCmdData()
{
    QByteArray data = cmdProcess->readAll();
    QString dataString = QString(data);
    qDebug() << "readCmdData " << dataString;

    if(dataString.contains("%") && dataString.contains("ETA")){
        bool ok;
        int value = dataString.mid(dataString.indexOf("%")-3, 3).toInt(&ok, 10);
        if(ok){
            progressDlg->show();
            progressDlg->setValue(value);
        }
        if(dataString.contains("100%")){
            progressDlg->hide();
            if(isBGM){
                QTimer::singleShot(100, this, SLOT(transferFileBGM()));
                isBGM = false;
            }
            else{
                QTimer::singleShot(100, this, SLOT(chmodFile()));
                QTimer::singleShot(500, this, SLOT(deleteFile()));
                QTimer::singleShot(1000, this, SLOT(finishModify()));
            }
        }
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::on_selectMusicBtn_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, "Select wav file",
                                                QDir::homePath() + "\\Music", "Audio Files (*.wav)");
    if(name.isEmpty())
        return;
    else{
        fileName = name;
        ui->fileLineEdit->setText(name.split("/").last());
        //qDebug() << fileName;
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::on_modifyBtn_clicked()
{
    copyFile();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::on_closeBtn_clicked()
{
    this->close();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::copyFile()
{
    //1. copy to XX.wav
    isBGM = false;
    QString targetFileName;
    if(ui->targetCB->currentIndex() == 0){
        targetFileName = "background.wav";
        isBGM = true;
    }
    else if(ui->targetCB->currentIndex() == 1){
        targetFileName = "OS.Robot Reception 1_1.wav";
    }
    else{
        targetFileName = QString("%1").arg(ui->targetCB->currentIndex()-1) + ".wav";
    }

    if (QFile::exists(QDir::currentPath() + "/" + targetFileName))
        QFile::remove(QDir::currentPath() + "/" + targetFileName);

    if(QFile::copy(fileName, QDir::currentPath() + "/" + targetFileName))
        QTimer::singleShot(100, this, SLOT(transferFile()));
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::transferFile()
{
    //2. transfer .wav to robot
    if(!cmdProcess->isOpen()){
        QString cmdStr;
        if(ui->targetCB->currentIndex() == 0){
            cmdStr = "echo y | pscp_32 -pw " + paraList.at(3) + " \"" + QDir::currentPath() + "/background.wav\" " + paraList.at(2) + "@"
                    + paraList.at(0) + ":/home/" + paraList.at(2) + "/mos_agv_sounds/9.wav" + "\n";
        }
        else if(ui->targetCB->currentIndex() == 1)
            cmdStr = "echo y | pscp_32 -pw " + paraList.at(3) + " \"" + QDir::currentPath() + "/OS.Robot Reception 1_1.wav\" " + paraList.at(2) + "@"
                    + paraList.at(0) + ":/home/" + paraList.at(2) + "/mos_agv_sounds" + "\n";
        else
            cmdStr = "echo y | pscp_32 -pw " + paraList.at(3) + " \"" + QDir::currentPath() + "/" + QString("%1").arg(ui->targetCB->currentIndex()-1) + ".wav\" "
                    + paraList.at(2) + "@" + paraList.at(0) + ":/home/" + paraList.at(2) + "/mos_agv_sounds" + "\n";
        //qDebug() << cmdStr;
        cmdProcess->start("cmd");
        cmdProcess->waitForStarted();
        cmdProcess->write(cmdStr.toLatin1());
        cmdProcess->closeWriteChannel();
        cmdProcess->waitForFinished();
        //QString stdOutput = QString::fromLocal8Bit(cmdProcess->readAllStandardOutput());
        QString stdError = QString::fromLocal8Bit(cmdProcess->readAllStandardError());
        //qDebug() << stdError;

        if(stdError.contains("Connection refused")){   //not install ssh on ubuntu
            warringDlg->setWindowTitle(tr("Critical"));
            warringDlg->setMessage(QString(tr("Receiving software is not installed on the robot!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }
        else if(stdError.contains("Access denied")){
            warringDlg->setWindowTitle(tr("Critical"));
            warringDlg->setMessage(QString(tr("Username or password wrong!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }
        else if(stdError.contains("'pscp'")){
            warringDlg->setWindowTitle(tr("Error"));
            warringDlg->setMessage(QString(tr("pscp.exe not exist on application directory!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }

        cmdProcess->kill();
        cmdProcess->close();
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::transferFileBGM()
{
    //2. transfer .wav to robot
    if(!cmdProcess->isOpen()){
        QString cmdStr;
        if(ui->targetCB->currentIndex() == 0){
            cmdStr = "echo y | pscp_32 -pw " + paraList.at(3) + " \"" + QDir::currentPath() + "/background.wav\" " + paraList.at(2) + "@"
                    + paraList.at(0) + ":/home/" + paraList.at(2) + "/mos_agv_sounds" + "\n";
        }
        //qDebug() << cmdStr;
        cmdProcess->start("cmd");
        cmdProcess->waitForStarted();
        cmdProcess->write(cmdStr.toLatin1());
        cmdProcess->closeWriteChannel();
        cmdProcess->waitForFinished();
        //QString stdOutput = QString::fromLocal8Bit(cmdProcess->readAllStandardOutput());
        QString stdError = QString::fromLocal8Bit(cmdProcess->readAllStandardError());
        //qDebug() << stdError;

        if(stdError.contains("Connection refused")){   //not install ssh on ubuntu
            warringDlg->setWindowTitle(tr("Critical"));
            warringDlg->setMessage(QString(tr("Receiving software is not installed on the robot!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }
        else if(stdError.contains("Access denied")){
            warringDlg->setWindowTitle(tr("Critical"));
            warringDlg->setMessage(QString(tr("Username or password wrong!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }
        else if(stdError.contains("'pscp'")){
            warringDlg->setWindowTitle(tr("Error"));
            warringDlg->setMessage(QString(tr("pscp.exe not exist on application directory!\nPlease contact the service staff")));
            warringDlg->show();
            deleteFile();
        }

        cmdProcess->kill();
        cmdProcess->close();
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::chmodFile()
{
    //3. chmod 775
    if(!cmdProcess->isOpen()){
        QString cmdStr;
        if(ui->targetCB->currentIndex() == 0){
            cmdStr = "plink_32 -no-antispoof " + paraList.at(2) + "@" + paraList.at(0)
                    + " -pw " + paraList.at(3) + " cd " + "/home/" + paraList.at(2) + "/mos_agv_sounds ;"
                    + " chmod 775 background.wav ; chmod 775 9.wav\n";
        }
        else if(ui->targetCB->currentIndex() == 1){
            cmdStr = "plink_32 -no-antispoof " + paraList.at(2) + "@" + paraList.at(0)
                    + " -pw " + paraList.at(3) + " cd " + "/home/" + paraList.at(2) + "/mos_agv_sounds ;"
                    + " chmod 775 \"OS.Robot Reception 1_1.wav\"";
        }
        else{
            cmdStr = "plink_32 -no-antispoof " + paraList.at(2) + "@" + paraList.at(0)
                    + " -pw " + paraList.at(3) + " cd " + "/home/" + paraList.at(2) + "/mos_agv_sounds ;"
                    + " chmod 775 " + QString("%1").arg(ui->targetCB->currentIndex()-1) + ".wav\n";
        }
        //qDebug() << cmdStr;
        cmdProcess->start("cmd");
        cmdProcess->waitForStarted();
        cmdProcess->write(cmdStr.toLatin1());
        cmdProcess->closeWriteChannel();
        cmdProcess->waitForFinished();
        //QString stdOutput = QString::fromLocal8Bit(cmdProcess->readAllStandardOutput());
        //QString stdError = QString::fromLocal8Bit(cmdProcess->readAllStandardError());
        //qDebug() << stdOutput;

        cmdProcess->kill();
        cmdProcess->close();
    }
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::deleteFile()
{
    //4. delete XX.wav
    /*if(!cmdProcess->isOpen()){
        QString cmdStr;
        if(ui->targetCB->currentIndex() == 0)
            cmdStr = "del " + QDir::currentPath() + "/background.wav\n";
        else if(ui->targetCB->currentIndex() == 1)
            cmdStr = "del " + QDir::currentPath() + "/\"OS.Robot Reception 1_1.wav\"\n";
        else
            cmdStr = "del " + QDir::currentPath() + "/" + QString("%1").arg(ui->targetCB->currentIndex()-1) + ".wav\n";
        cmdStr.replace("/", "\\");
        //qDebug() << cmdStr;
        cmdProcess->start("cmd");
        cmdProcess->waitForStarted();
        cmdProcess->write(cmdStr.toLatin1());
        cmdProcess->closeWriteChannel();
        cmdProcess->waitForFinished();
        QString stdOutput = QString::fromLocal8Bit(cmdProcess->readAllStandardOutput());
        QString stdError = QString::fromLocal8Bit(cmdProcess->readAllStandardError());
        //qDebug() << "deleteFile  " << stdOutput << stdError;

        cmdProcess->kill();
        cmdProcess->close();
    }*/

    QString targetFileName;
    if(ui->targetCB->currentIndex() == 0)
        targetFileName = "background.wav";
    else if(ui->targetCB->currentIndex() == 1)
        targetFileName = "OS.Robot Reception 1_1.wav";
    else
        targetFileName = QString("%1").arg(ui->targetCB->currentIndex()-1) + ".wav";

    if (QFile::exists(QDir::currentPath() + "/" + targetFileName))
        QFile::remove(QDir::currentPath() + "/" + targetFileName);
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::finishModify()
{
    warringDlg->setWindowTitle(tr("Ok"));
    warringDlg->setMessage(QString(tr("Already change music!")));
    warringDlg->show();
}

/*
*Function Description :
*Parameters : 	NONE
*Returns : 		void
*/
void SoundModifyDialog::cancelTransfer()    //QprogressDialog cancel and close signal
{
    cmdProcess->terminate();
    cmdProcess->kill();
    cmdProcess->close();
    QTimer::singleShot(15000, this, SLOT(deleteFile()));
}
