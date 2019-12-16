#include "dynparamdialog.h"
#include "ui_dynparamdialog.h"

dynparamDialog::dynparamDialog(QStringList valueList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dynparamDialog)
{
    ui->setupUi(this);
    oriValueList = valueList;
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(3);
    sizePolicy.setVerticalStretch(0);
    bool ok;
    for(int i=0; i<SIZE_OF_ARRAY(dynparamTable); i++){
        if(dynparamTable[i][0] == "bool"){
            QLabel *nameLabel = new QLabel(this);
            nameLabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_nameLabel");
            nameLabel->setText(dynparamTable[i][1] + "/" + dynparamTable[i][2]);
            ui->gridLayout->addWidget(nameLabel, i, 0, 1, 1, Qt::AlignLeft);

            QLabel *minlabel = new QLabel(this);
            minlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_minlabel");
            minlabel->setText(dynparamTable[i][3]);
            ui->gridLayout->addWidget(minlabel, i, 1, 1, 1, Qt::AlignLeft);

            QLabel *maxlabel = new QLabel(this);
            maxlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_maxlabel");
            maxlabel->setText(dynparamTable[i][4]);
            ui->gridLayout->addWidget(maxlabel, i, 3, 1, 1, Qt::AlignRight);

            QCheckBox *checkBox = new QCheckBox(this);
            checkBox->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_checkBox");
            ui->gridLayout->addWidget(checkBox, i, 2, 1, 1, Qt::AlignHCenter);

            if(valueList.at(i) == "true")
                checkBox->setChecked(true);
            else
                checkBox->setChecked(false);
        }
        else if(dynparamTable[i][0] == "int"){
            QLabel *nameLabel = new QLabel(this);
            nameLabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_nameLabel");
            nameLabel->setText(dynparamTable[i][1] + "/" + dynparamTable[i][2]);
            ui->gridLayout->addWidget(nameLabel, i, 0, 1, 1, Qt::AlignLeft);

            QLabel *minlabel = new QLabel(this);
            minlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_minlabel");
            minlabel->setText(dynparamTable[i][3]);
            ui->gridLayout->addWidget(minlabel, i, 1, 1, 1, Qt::AlignLeft);

            QLabel *maxlabel = new QLabel(this);
            maxlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_maxlabel");
            maxlabel->setText(dynparamTable[i][4]);
            ui->gridLayout->addWidget(maxlabel, i, 3, 1, 1, Qt::AlignRight);

            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_spinBox");
            spinBox->setMinimum(dynparamTable[i][3].toInt(&ok, 10));
            spinBox->setMaximum(dynparamTable[i][4].toInt(&ok, 10));
            spinBox->setSizePolicy(sizePolicy);
            spinBox->setFixedWidth(100);
            ui->gridLayout->addWidget(spinBox, i, 2, 1, 1, Qt::AlignHCenter);

            spinBox->setValue(valueList.at(i).toInt());
        }
        else if(dynparamTable[i][0] == "float"){
            QLabel *nameLabel = new QLabel(this);
            nameLabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_nameLabel");
            nameLabel->setText(dynparamTable[i][1] + "/" + dynparamTable[i][2]);
            ui->gridLayout->addWidget(nameLabel, i, 0, 1, 1, Qt::AlignLeft);

            QLabel *minlabel = new QLabel(this);
            minlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_minlabel");
            minlabel->setText(dynparamTable[i][3]);
            ui->gridLayout->addWidget(minlabel, i, 1, 1, 1, Qt::AlignLeft);

            QLabel *maxlabel = new QLabel(this);
            maxlabel->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_maxlabel");
            maxlabel->setText(dynparamTable[i][4]);
            ui->gridLayout->addWidget(maxlabel, i, 3, 1, 1, Qt::AlignRight);

            QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(this);
            doubleSpinBox->setObjectName(dynparamTable[i][1] + "/" + dynparamTable[i][2] + "_doubleSpinBox");
            doubleSpinBox->setMinimum(dynparamTable[i][3].toFloat(&ok));
            doubleSpinBox->setMaximum(dynparamTable[i][4].toFloat(&ok));
            doubleSpinBox->setSizePolicy(sizePolicy);
            doubleSpinBox->setFixedWidth(100);
            ui->gridLayout->addWidget(doubleSpinBox, i, 2, 1, 1, Qt::AlignHCenter);

            doubleSpinBox->setValue(valueList.at(i).toDouble());
        }
    }
    ui->gridLayout->setColumnStretch(0, 2);
    ui->gridLayout->setColumnStretch(1, 1);
    ui->gridLayout->setColumnStretch(2, 3);
    ui->gridLayout->setColumnStretch(3, 1);
}

dynparamDialog::~dynparamDialog()
{
    delete ui;
}

void dynparamDialog::on_closeBtn_clicked()
{
    this->close();
}

void dynparamDialog::on_writeBtn_clicked()
{
    QString cmdStr;

    for(int i=0; i<SIZE_OF_ARRAY(dynparamTable); i++){
        if(dynparamTable[i][0] == "bool"){
            QCheckBox *cb = qobject_cast<QCheckBox *>(ui->gridLayout->itemAt((i+1)*4-1)->widget());
            if(cb != nullptr){
                if(oriValueList.at(i) == "true" && !cb->checkState()
                        || oriValueList.at(i) == "false" && cb->checkState()){
                    if(cb->checkState()){
                        cmdStr.append("rosrun dynamic_reconfigure dynparam set " + dynparamTable[i][1] + " " + dynparamTable[i][2] + " true && ");
                        oriValueList.replace(i, "true");
                    }
                    else{
                        cmdStr.append("rosrun dynamic_reconfigure dynparam set " + dynparamTable[i][1] + " " + dynparamTable[i][2] + " false && ");
                        oriValueList.replace(i, "false");
                    }
                }
            }
        }
        else if(dynparamTable[i][0] == "int"){
            QSpinBox *sb = qobject_cast<QSpinBox *>(ui->gridLayout->itemAt((i+1)*4-1)->widget());
            if(sb != nullptr){
                if(oriValueList.at(i).toInt() != sb->value()){
                    cmdStr.append("rosrun dynamic_reconfigure dynparam set " + dynparamTable[i][1] + " " + dynparamTable[i][2]
                            + " " + QString::number(sb->value(), 10) + " && ");
                    oriValueList.replace(i, QString::number(sb->value(), 10));
                }
            }
        }
        else if(dynparamTable[i][0] == "float"){
            QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox *>(ui->gridLayout->itemAt((i+1)*4-1)->widget());
            if(dsb != nullptr){
                if(oriValueList.at(i).toDouble() != dsb->value()){
                    cmdStr.append("rosrun dynamic_reconfigure dynparam set " + dynparamTable[i][1] + " " + dynparamTable[i][2]
                            + " " + QString::number(dsb->value()) + " && ");
                    oriValueList.replace(i, QString::number(dsb->value()));
                }
            }
        }
    }

    cmdStr.remove(cmdStr.length()-4, 4);
    emit setDynparam(cmdStr);
}
