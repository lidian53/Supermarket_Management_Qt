#include "showinforlabel.h"
#include <QPainter>

ShowInforLabel::ShowInforLabel(QWidget *parent) :
    QLabel(parent)
{
    _opacity = 1.0;         //私有成员值，存储窗体的不透明度,0.0-1.0，1.0为不透明
}

void ShowInforLabel::setopacity(double value)           //当执行自定义属性动画时，动画会自动调用该函数
{
    _opacity = value;           //设置不透明度
    update();               //重绘
}

void ShowInforLabel::setWindowVisable()         //设置窗体为不可见，用于自定义动画时，结束时调用
{
    this->setVisible(false);
    _opacity = 1.0;             //重新设置不透明度,为下一次属性动画准备
}

void ShowInforLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int width = fontMetrics().width(this->text()) + 20;//根据文本，提出宽度
    int height = 25;
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setBrush(Qt::white);
    QFont font;
    font.setPointSize(9);
    painter.setFont(font);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setOpacity(_opacity);       //设置不透明度
    painter.drawRoundedRect(QRectF(2,10,width,height),20,20);//绘制一个圆矩形
    static const QPointF points[3] = {
        QPointF(20,0),
        QPointF(25,20),
        QPointF(35,20)
    };
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawPolygon(points,3);      //绘制一个三角形
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRectF(2,10,width,height),20,20);   //使用白色填充圆矩形，剔除矩形中多余的东西
    painter.setPen(Qt::black);
    painter.drawText(12,25,this->text());       //绘制要显示的文本
}
