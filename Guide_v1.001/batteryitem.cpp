#include "batteryitem.h"
#include <QPainter>
#include <QDebug>

BatteryItem::BatteryItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

BatteryItem::BatteryItem(QWidget *parent, int value, int color /*= 0*/)
    : QWidget(parent)
{
    //ui.lab_value->setText("dd");
    //setFixedSize(26, 16);
    //setFixedSize(100, 50);
    setBatteryColor(color);
    setBatteryValue(value);
    setShowText(1);
}

void BatteryItem::setBatteryValue(int value)
{
    if(value != -1){
        if (value < 0 )
            value = 0;
        else if (value > 100)
            value = 100;

        if(value >= 40)
            setBatteryColor(0);
        else if (value < 40 && value >= 20)
            setBatteryColor(1);
        else if (value < 20)
            setBatteryColor(2);
        setShowText(1);
    }
    else{
        setBatteryColor(3);
        setShowText(2, "Null");
    }
    m_value = value;
    update();
}

void BatteryItem::setBatteryColor(int color)
{
    if (color < 0 || color > 3)
        color = 0;

    //QColor(136, 205, 112) - 翠绿1
    //QColor(137, 249, 83) - 翠绿2
    switch (color)
    {
    case 0://green
        m_color = QColor(0, 200, 0);
        break;
    case 1://yellow
        m_color = QColor(218, 165, 32);
        //m_color = QColor(255, 255, 0);
        break;
    case 2://red
        m_color = QColor(255, 0, 0);
        break;
    case 3://gray
        m_color = QColor(200, 200, 200);
        break;
    }
    update();
}

void BatteryItem::setShowText(int flg/*=0*/, QString str/*=""*/)
{
    m_showTextFlg = flg;
    m_showText = str;
    update();
}

BatteryItem::~BatteryItem()
{

}

void BatteryItem::paintEvent(QPaintEvent *event)
{

    QSize itemSize = this->size();
    const int margin = 10;//外框
    int w = itemSize.width();
    int h = itemSize.height();

    int x0, y0, w0, h0;//外框數據
    int x1, y1, w1, h1;//内框數據 - 電量
    int x2, y2, w2, h2;//内框數據 - 電池頭

    //外框
    x0 = 1; y0 = x0;
    w0 = w - 2 * x0 - margin;
    h0 = h - 2 * y0;

    //内框填充
    x1 = 4; y1 = x1;
    w1 = w - 2 * x1 - 4;
    h1 = h - 2 * y1;

    //電池頭
    x2 = x0 + w0 ;
    y2 = h / 3;
    w2 = margin;
    h2 = h / 3;

    QPainter painter(this);
    QPen pen;

    QFont font = painter.font();
    font.setPointSize(font.pointSize() * 2);
    painter.setFont(font);

    painter.setPen(m_color);//畫外框的颜色
    pen = painter.pen();
    pen.setWidth(2);//線框寬度
    painter.setPen(pen);

    //畫外框矩形
    //左上點[x] [y] [width] [height] [x半徑] [y半徑]
    //此時沒有填充颜色
    painter.drawRoundedRect(x0, y0, w0, h0, 2, 2);

    //畫內部電量 和 電池的頭
    //填充的颜色
    painter.setBrush(m_color);
    //電池頭
    //painter.drawRect(x2, y2, w2, h2);
    painter.drawRoundedRect(x2, y2, w2, h2, 1, 1);

    //左上點[x] [y] [width] [height]
    //int w1_current = m_value*0.01*(w1);
    int tenPartValue = m_value/10 * 10;
    if(m_value == -1)
            tenPartValue = 0;
    else if(m_value < 100)
        tenPartValue = (m_value/10 + 1) * 10;

    int w1_current = tenPartValue*0.01*(w1);
    //qDebug() << m_value << tenPartValue;

    //根据數值設置電量
    painter.drawRect(x1, y1, w1_current, h1);


    //painter.setPen(QColor(0, 0, 0));//畫外框的颜色
    painter.setPen(QColor(255, 255, 255));//畫外框的颜色

    /*switch (m_showTextFlg)
    {
    case 0://0:不顯示文字
        //do nothing
        break;
    case 1://1:顯示百分比
        painter.drawText(x0, y0, w0, h0,
                         Qt::AlignHCenter | Qt::AlignVCenter,
                         QString::asprintf("%d %%", m_value));
        break;
    case 2://2:顯示自定義文字
        painter.drawText(x0, y0, w0, h0,
                         Qt::AlignHCenter | Qt::AlignVCenter,
                         m_showText);
        break;
    }*/
    QWidget::paintEvent(event);
}
