#include "collectdlg.h"
#include "ui_collectdlg.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <qmath.h>

collectDlg::collectDlg(double pay, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::collectDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->priceLineEdit->setText(QString::number(pay));
    ui->collectLineEdit->setText(QString::number(pay));
    ui->collectLineEdit->selectAll();
    ui->collectLineEdit->setFocus();
    ui->OkBtn->setDefault(true);
    QDoubleValidator *validator = new QDoubleValidator(this);
    validator->setDecimals(1);
    validator->setBottom(0);
    ui->collectLineEdit->setValidator(validator);
}

collectDlg::~collectDlg()
{
    delete ui;
}

void collectDlg::accept()
{
    double pr = ui->changeLineEdit->text().toDouble();
    if(pr < 0)
    {
        QMessageBox::warning(this,tr("警告"),tr("实付不得小于总价。"),QMessageBox::Ok);
        return;
    }
    QDialog::accept();
}

double collectDlg::getPayPrice() const
{
    return ui->collectLineEdit->text().toDouble();
}

void collectDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton  && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        event->accept();
        isDrag = true;
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void collectDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 50 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void collectDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void collectDlg::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10,10,this->width()-20,this->height()-20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillPath(path,QBrush(Qt::white));

    QColor color(0,0,0,55);
    for(int i = 0;i < 9;i++)
    {
        QPainterPath pathShadow;
        pathShadow.setFillRule(Qt::WindingFill);
       // pathShadow.addRoundRect(10-i,10-i,this->width()-(10-i)*2,this->height()-(10-i)*2,5);
        pathShadow.addRect(10-i,10-i,this->width()-(10-i)*2,this->height()-(10-i)*2);
        color.setAlpha(155-qSqrt(i)*55);
        painter.setPen(color);
        painter.drawPath(pathShadow);
    }
}

void collectDlg::on_collectLineEdit_textChanged(const QString &arg1)
{
    double pay = ui->priceLineEdit->text().toDouble();      //应付
    double factPay = arg1.toDouble();
    ui->changeLineEdit->setText(QString::number(factPay - pay));
}
