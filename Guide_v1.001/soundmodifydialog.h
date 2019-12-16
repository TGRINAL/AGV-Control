#ifndef SOUNDMODIFYDIALOG_H
#define SOUNDMODIFYDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QTextCodec>
#include <QTimer>
#include <QProgressDialog>

#include "warringdialog.h"

namespace Ui {
class SoundModifyDialog;
}

class SoundModifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoundModifyDialog(QStringList list, QWidget *parent = nullptr);
    ~SoundModifyDialog();

private slots:
    void readCmdData();
    void on_modifyBtn_clicked();
    void on_selectMusicBtn_clicked();
    void on_closeBtn_clicked();

    void copyFile();
    void transferFile();
    void transferFileBGM();
    void chmodFile();
    void deleteFile();
    void finishModify();
    void cancelTransfer();

private:
    Ui::SoundModifyDialog *ui;
    QStringList paraList;
    QString fileName;
    QProcess *cmdProcess;
    bool isBGM;
    QProgressDialog *progressDlg;
    WarringDialog *warringDlg;
};

#endif // SOUNDMODIFYDIALOG_H
