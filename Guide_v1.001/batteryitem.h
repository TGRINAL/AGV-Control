#ifndef BATTERYITEM_H
#define BATTERYITEM_H

#include <QWidget>
#include "ui_batteryitem.h"

class BatteryItem : public QWidget
{
    Q_OBJECT

public:
    BatteryItem(QWidget *parent = 0);
    BatteryItem(QWidget *parent, int value, int color = 0);
    void setBatteryValue(int value);
    void setBatteryColor(int color);
    void setShowText(int flg=0, QString str="");
    ~BatteryItem();

    //virtual QPaintEngine * paintEngine() const override;

private:
    Ui::BatteryItem ui;
    int m_value;
    QColor m_color = QColor(0,255,0);
    int m_showTextFlg = 0;
    QString m_showText;

protected:
    virtual void paintEvent(QPaintEvent * event) override;

};

#endif // BATTERYITEM_H
