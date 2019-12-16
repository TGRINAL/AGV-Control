#include "joystickdialog.h"
#include "ui_joystickdialog.h"

JoystickDialog::JoystickDialog(bool flicker, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoystickDialog),
    flickerFlag(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint);
    teleopMode = TELEOP_TYPE::LINEAR;
    ui->linearRb->setChecked(true);
    flickerFlag = flicker;

    backgroundTimer = new QTimer;
    backgroundTimer->setInterval(1000);
    connect(backgroundTimer, &QTimer::timeout, this, &JoystickDialog::handleBackgroundTimeout);
    if(flickerFlag)
        backgroundTimer->start();
    else{
        backgroundTimer->stop();
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");

    }
    ui->modeGb->setVisible(false);
    ui->linearRb->setVisible(false);
    ui->jogRb->setVisible(false);
    ui->velLabel->setText(tr("Linear: %1 m/s\nAngular: %2 degree/s").arg(linearValue).arg((int)(180.0*angularValue/M_PI)));
    ui->showBtn->icon().setThemeName("hide");
    on_showBtn_clicked();
}

JoystickDialog::~JoystickDialog()
{
    emit setTeleopMode(teleopMode);
    emit setKeyPriorityFlag(true);
    delete backgroundTimer;
    delete ui;
}

void JoystickDialog::setFlicker(bool flag)
{
    flickerFlag = flag;
    if(flickerFlag)
        backgroundTimer->start();
    else{
        backgroundTimer->stop();
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    }
}

void JoystickDialog::handleBackgroundTimeout()
{
    static bool pingpong = true;
    if(pingpong)
        this->setStyleSheet("QDialog{background:rgba(255,0,0,255);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    else
        this->setStyleSheet("QDialog{background:rgba(32,80,96,100);color:white;}"
                            "QPushButton{color:white;background-color:#27a9e3;border-width:0px;border-radius:3px;}"
                            "QLabel,QRadioButton,QCheckBox{color:white; border-radius:3px;}"
                            "QPushButton:hover, QComboBox:hover{ background-color: #66c011;}");
    pingpong = !pingpong;
}

void JoystickDialog::on_closeBtn_clicked()
{
    this->close();
}

void JoystickDialog::on_forwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Forward);
}

void JoystickDialog::on_backwardBtn_pressed()
{
    emit updateVel(VEL_TYPE::Backward);
}

void JoystickDialog::on_leftBtn_pressed()
{
    emit updateVel(VEL_TYPE::Left);
}

void JoystickDialog::on_rightBtn_pressed()
{
    emit updateVel(VEL_TYPE::Right);
}

void JoystickDialog::on_forwardBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

void JoystickDialog::on_backwardBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

void JoystickDialog::on_leftBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

void JoystickDialog::on_rightBtn_released()
{
    if(teleopMode == TELEOP_TYPE::JOG)
        emit updateVel(VEL_TYPE::Stop);
}

void JoystickDialog::on_stopBtn_clicked()
{
    emit updateVel(VEL_TYPE::Stop);
}

void JoystickDialog::on_jogRb_toggled(bool checked)
{
    if(checked)
        teleopMode = TELEOP_TYPE::JOG;
    else
        teleopMode = TELEOP_TYPE::LINEAR;
    emit updateVel(VEL_TYPE::Stop);
    emit setTeleopMode(teleopMode);
}

void JoystickDialog::on_abortBtn_clicked()
{
    emit abortMovebase();
}

void JoystickDialog::on_safeBtn_clicked()
{
    if(ui->safeBtn->text() == tr("Safe Stop")){
        WarringDialog *dlg = new WarringDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setMessage(tr("Are you sure to turn off the avoidance function?"));
        dlg->setModal(true);
        dlg->setFixedSize(dlg->width(), dlg->height());
        dlg->move(this->pos().x()+this->size().width()/2-dlg->size().width()/2, this->pos().y()+this->size().height()/2-dlg->size().height()/2);

        if(dlg->exec()){
            ui->safeBtn->setText(tr("No safe stop"));
            ui->safeBtn->setStyleSheet("background-color: rgb(255, 0, 0);");
            emit setKeyPriorityFlag(false);
        }
    }
    else if(ui->safeBtn->text() == tr("No safe stop")){
        ui->safeBtn->setText(tr("Safe Stop"));
        ui->safeBtn->setStyleSheet("background-color: rgb(0, 200, 0);");
        emit setKeyPriorityFlag(true);
    }
}

void JoystickDialog::getVel(float linear, float angular)
{
    linearValue = linear;
    angularValue = angular;

    ui->velLabel->setText(tr("Linear: %1 m/s\nAngular: %2 degree/s").arg(linearValue).arg((int)(180.0*angularValue/M_PI)));
}

void JoystickDialog::changeEvent(QEvent *event)
{
    if(0 != event) {
        switch(event->type()) {
        case QEvent::LanguageChange:{
            ui->retranslateUi(this);
            break;
        }
        default:
            break;
        }
        QDialog::changeEvent(event);
    }
}

void JoystickDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_S)
        emit updateVel(VEL_TYPE::Stop);
    else if(event->key() == Qt::Key_W)
        emit updateVel(VEL_TYPE::Forward);
    else if(event->key() == Qt::Key_X)
        emit updateVel(VEL_TYPE::Backward);
    else if(event->key() == Qt::Key_A)
        emit updateVel(VEL_TYPE::Left);
    else if(event->key() == Qt::Key_D)
        emit updateVel(VEL_TYPE::Right);
}

void JoystickDialog::on_ledLeftBtn_clicked()
{
    if(ui->ledComboBox->currentIndex() > 0)
        ui->ledComboBox->setCurrentIndex(ui->ledComboBox->currentIndex()-1);
}

void JoystickDialog::on_ledRightBtn_clicked()
{
    if(ui->ledComboBox->currentIndex() < ui->ledComboBox->count()-1)
        ui->ledComboBox->setCurrentIndex(ui->ledComboBox->currentIndex()+1);
}

void JoystickDialog::on_audioLeftBtn_clicked()
{
    if(ui->audioComboBox->currentIndex() > 0)
        ui->audioComboBox->setCurrentIndex(ui->audioComboBox->currentIndex()-1);
}

void JoystickDialog::on_audioRightBtn_clicked()
{
    if(ui->audioComboBox->currentIndex() < ui->audioComboBox->count()-1)
        ui->audioComboBox->setCurrentIndex(ui->audioComboBox->currentIndex()+1);
}

void JoystickDialog::on_ledPlayBtn_clicked()
{
    emit showAction(0, ui->ledComboBox->currentIndex());
}

void JoystickDialog::on_audioPlayBtn_clicked()
{
    emit showAction(1, ui->audioComboBox->currentIndex());
}

void JoystickDialog::on_showBtn_clicked()
{
    if(ui->showBtn->icon().themeName() == "show"){
        ui->showBtn->icon().setThemeName("hide");
        QPixmap pixmap("Img/arrow_up.png");
        QIcon icon(pixmap);
        ui->showBtn->setIcon(icon);
        ui->showBtn->setIconSize(QSize(32, 32));

        for (int i = 0; i < ui->showLayout->count(); ++i)
        {
            QWidget *widget = ui->showLayout->itemAt(i)->widget();
            if (widget != NULL)
                widget->setVisible(true);
        }
    }
    else{
        ui->showBtn->icon().setThemeName("show");
        QPixmap pixmap("Img/arrow_down.png");
        QIcon icon(pixmap);
        ui->showBtn->setIcon(icon);
        ui->showBtn->setIconSize(QSize(32, 32));
        for (int i = 0; i < ui->showLayout->count(); ++i)
        {
            QWidget *widget = ui->showLayout->itemAt(i)->widget();
            if (widget != NULL)
                widget->setVisible(false);
        }
    }
}

void JoystickDialog::on_audioLoopBtn_clicked()
{
    emit showAction(2, ui->audioComboBox->currentIndex());
}
