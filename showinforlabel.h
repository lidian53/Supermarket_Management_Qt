#ifndef SHOWINFORLABEL_H
#define SHOWINFORLABEL_H

#include <QLabel>
/*
子类化一个QLabel
功能：显示一些错误信息，界面会更加友好
*/


class ShowInforLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(double opacity READ opacity WRITE setopacity)   //自定义一个属性，opacity设置窗体的透明度，可以执行自定义的属性动画
public:
    explicit ShowInforLabel(QWidget *parent = 0);
    double opacity() { return _opacity;}            //自定义属性方法
    void setopacity(double value);
public slots:
    void setWindowVisable();
protected:
    void paintEvent(QPaintEvent *event);
    double _opacity;
    
};

#endif // SHOWINFORLABEL_H
