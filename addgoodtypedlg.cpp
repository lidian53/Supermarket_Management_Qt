#include "addgoodtypedlg.h"
#include "ui_addgoodtypedlg.h"
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

AddGoodTypeDlg::AddGoodTypeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddGoodTypeDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

AddGoodTypeDlg::~AddGoodTypeDlg()
{
    delete ui;
}

void AddGoodTypeDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 70)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void AddGoodTypeDlg::paintEvent(QPaintEvent *event)
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

void AddGoodTypeDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void AddGoodTypeDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void AddGoodTypeDlg::on_cancelBtn_clicked()
{
    this->reject();
}

void AddGoodTypeDlg::on_okBtn_clicked()
{
    QString typeName = ui->nameLineEdit->text();
    if(typeName.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请输入要添加的商品类别名！"),QMessageBox::Ok);
        ui->nameLineEdit->setFocus();
        return;
    }

    int id = 0;
    QSqlQuery query;
    query.exec("SELECT MAX(id) FROM goodsTypeTb");
    while(query.next())
    {
        id = query.value(0).toInt();
    }
    id = id == 0 ? id: (id+1);  //如果已经有数据了，就将id值加一
    query.prepare("INSERT INTO goodsTypeTb VALUES(:id,:typename)");
    query.bindValue(":id",id);
    query.bindValue(":typename",typeName);
    if(query.exec())
    {
        QMessageBox::information(this,tr("提示"),tr("添加成功"),QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(this,tr("严重错误"),tr("抱歉！添加失败。\n错误原因：%1")
                              .arg(query.lastError().text()));
        ui->nameLineEdit->setFocus();
        return;
    }
    this->accept();
}

void AddGoodTypeDlg::on_closeBtn_clicked()
{
    this->reject();
}
