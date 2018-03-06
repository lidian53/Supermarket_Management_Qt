#include "choicegoodsdlg.h"
#include "ui_choicegoodsdlg.h"
#include <QSqlQuery>
#include <QStringListModel>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QPlastiqueStyle>
#include <qmath.h>
#include <QSqlTableModel>
#include <QMessageBox>

ChoiceGoodsDlg::ChoiceGoodsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoiceGoodsDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->listView->setStyle(new QPlastiqueStyle);
    this->setAttribute(Qt::WA_TranslucentBackground);
    typeModel = new QStringListModel(this);

    ui->listView->setModel(typeModel);
    loadList();
    model = new QSqlTableModel;
    model->setTable("goodsNameTb");
    model->setHeaderData(GoodsName_Id,Qt::Horizontal,tr("条码编号"));
    model->setHeaderData(GoodsName_Name,Qt::Horizontal,tr("商品名"));
    model->setHeaderData(GoodsName_Retail,Qt::Horizontal,tr("零售价"));
    model->select();
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnHidden(GoodsName_TypeId,true);
    ui->tableView->setColumnHidden(GoodsName_Unit,true);
    ui->tableView->setColumnHidden(GoodsName_Bid,true);
    ui->tableView->setColumnHidden(GoodsName_Format,true);
    ui->tableView->setColumnHidden(GoodsName_Stock,true);
    ui->tableView->setColumnHidden(GoodsName_Note,true);
    ui->tableView->setColumnWidth(GoodsName_Retail,50);
}

ChoiceGoodsDlg::~ChoiceGoodsDlg()
{
    delete ui;
}

QString ChoiceGoodsDlg::getChoiceGoodsNum() const
{
    if(goodsNum.isEmpty())
        return QString("");
    return goodsNum;
}

void ChoiceGoodsDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        event->accept();
        isDrag = true;
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void ChoiceGoodsDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 50 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void ChoiceGoodsDlg::mouseReleaseEvent(QMouseEvent *)
{
    isDrag = false;
}

void ChoiceGoodsDlg::paintEvent(QPaintEvent *event)
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

void ChoiceGoodsDlg::accept()//重新实现，当单击确定或者双击表格项时触发
{
    int row = ui->tableView->currentIndex().row();
    if(row != -1)       //如果没有选择值为-1
    {
        //将商品编号存储到私有成员中
        goodsNum = ui->tableView->model()->index(row,0).data(Qt::DisplayRole).toString();
        QDialog::accept();      //并调用父类方法以关闭窗体
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),tr("请选择需要添加的商品"),QMessageBox::Ok);

    }
}

void ChoiceGoodsDlg::loadList()     //加载所有商品
{
    QStringList list;
    QSqlQuery query;
    query.exec(tr("select typename from goodsTypeTb"));
    while(query.next())
    {
        list << query.value(0).toString();
    }
    typeModel->setStringList(list);
}

void ChoiceGoodsDlg::on_lineEdit_textChanged(const QString &arg1)   //商品类别搜索
{
    if(arg1.isEmpty())
    {
        loadList();
        return;
    }
    QSqlQuery query;
    QStringList list;
    query.exec(tr("select *from goodsTypeTb where typename like '%1%'").arg(arg1));
    while(query.next())
    {
        list << query.value(1).toString();
    }
    typeModel->setStringList(list);
}

void ChoiceGoodsDlg::on_listView_clicked(const QModelIndex &index)  //商品类别选择
{
    ui->lineEdit_2->clear();
    QString text = index.data(Qt::DisplayRole).toString();
    if(text == tr("查看所有商品"))
    {
        model->setFilter(tr(""));
        model->select();
        return;
    }
    QSqlQuery query;
    query.exec(tr("select id from goodsTypeTb where typename = '%1'").arg(text));
    query.next();
    int id = query.value(0).toInt();
    model->setFilter(tr("typeId = '%1'").arg(id));
    model->select();

}

void ChoiceGoodsDlg::on_lineEdit_2_textChanged(const QString &arg1)     //商品名称或编号搜索
{
    model->setFilter(tr("num like '%1%' or name like '%1%'").arg(arg1));
    model->select();
    if(ui->tableView->model()->rowCount() != 0)
        ui->tableView->selectRow(0);
}

void ChoiceGoodsDlg::on_tableView_doubleClicked(const QModelIndex &index)   //
{
    accept();
}

