#include "resetorigindialog.h"
#include "ui_resetorigindialog.h"

ResetOriginDialog::ResetOriginDialog(ORIGINMODE mode, QList<Origin *> originList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetOriginDialog)
{
    ui->setupUi(this);
    m_mode = mode;
    if(m_mode == ORIGINMODE::RESETORIGIN){
        this->setWindowTitle(tr("Origin Initialization"));
        ui->poseUpdateBtn->setText(tr("Pose Update"));
    }
    else if(m_mode == ORIGINMODE::STARTORIGIN){
        this->setWindowTitle(tr("Start From ?"));
        ui->poseUpdateBtn->setText(tr("Beginning"));
    }

    for(int i=0; i<originList.count(); i++){
        QPushButton *btn = new QPushButton(tr("Origin %1 \n(").arg(i+1) + originList.at(i)->name + ")");
        btn->setFixedHeight(50);
        QFont font("微軟正黑體", 16);
        btn->setFont(font);
        btn->setObjectName(QString::number(i+1));
        connect(btn, &QPushButton::clicked, this, &ResetOriginDialog::buttonClick);
        ui->originLayout->addWidget(btn);
    }
}

ResetOriginDialog::ResetOriginDialog(ORIGINMODE mode, QList<ScriptButton *> buttonList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetOriginDialog)
{
    ui->setupUi(this);
    m_mode = mode;
    if(m_mode == ORIGINMODE::RESETORIGIN){
        this->setWindowTitle(tr("Origin Initialization"));
        ui->poseUpdateBtn->setText(tr("Pose Update"));
    }
    else if(m_mode == ORIGINMODE::STARTORIGIN){
        this->setWindowTitle(tr("Execute which ?"));
        ui->poseUpdateBtn->setVisible(false);
    }
    int row = 0, column = 0;
    for(int i=0; i<buttonList.count(); i++){
        QPushButton *btn = new QPushButton(buttonList.at(i)->buttonName);
        btn->setFixedHeight(50);
        QFont font("微軟正黑體", 16);
        btn->setFont(font);
        btn->setObjectName(QString::number(i));
        connect(btn, &QPushButton::clicked, this, &ResetOriginDialog::buttonClick);
        ui->originLayout->addWidget(btn, row, column);
        column++;
        if(column > 3){
            row++;
            column = 0;
        }
    }
}

ResetOriginDialog::~ResetOriginDialog()
{
    delete ui;
}

void ResetOriginDialog::on_poseUpdateBtn_clicked()
{
    if(m_mode == ORIGINMODE::RESETORIGIN)
        emit poseUpdte();
    else if(m_mode == ORIGINMODE::STARTORIGIN){
        emit startFromWhere(-1);
        close();
    }
}

void ResetOriginDialog::on_closeBtn_clicked()
{
    close();
}

void ResetOriginDialog::buttonClick()
{
    QObject *obj = QObject::sender();
    QStringList strLines = obj->objectName().split( "\n", QString::SkipEmptyParts );
    bool ok;
    int idx = strLines.first().toInt(&ok);

    if(m_mode == ORIGINMODE::RESETORIGIN)
        emit resetOrigin(idx);
    else if(m_mode == ORIGINMODE::STARTORIGIN)
        emit startScript(idx);
    close();
}
