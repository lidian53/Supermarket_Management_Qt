#include "supplierdlg.h"
#include "ui_supplierdlg.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include <QSqlTableModel>
#include "addsupplierdlg.h"
#include <QClipboard>
#include <QSqlRecord>
#include <QSqlQuery>

SupplierDlg::SupplierDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SupplierDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    initialisePage();
}


void SupplierDlg::initialisePage()  //初始化
{
    supplierModel = new QSqlTableModel(this);
    supplierModel->setTable("supplierTb");
    supplierModel->select();

    supplierModel->setHeaderData(Supplier_Name,Qt::Horizontal,tr("供应商名称"));
    supplierModel->setHeaderData(Supplier_Contact,Qt::Horizontal,tr("联系人"));
    supplierModel->setHeaderData(Supplier_Telephone,Qt::Horizontal,tr("联系电话"));
    supplierModel->setHeaderData(Supplier_Address,Qt::Horizontal,tr("地址"));
    supplierModel->setHeaderData(Supplier_Note,Qt::Horizontal,tr("备注"));

    ui->tableView->setModel(supplierModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setColumnHidden(Supplier_Id,true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->resizeColumnsToContents();

    copyNameAction = new QAction(tr("复制供应商名"),this);
    connect(copyNameAction,SIGNAL(triggered()),this,SLOT(copyTheNameSlot()));
    copyNumberAction = new QAction(tr("复制电话号码"),this);
    connect(copyNumberAction,SIGNAL(triggered()),this,SLOT(copyTheNumberSlot()));
    ui->tableView->addAction(copyNameAction);
    ui->tableView->addAction(copyNumberAction);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
}

SupplierDlg::~SupplierDlg()
{
    delete ui;
}

void SupplierDlg::paintEvent(QPaintEvent *event)
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

void SupplierDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void SupplierDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void SupplierDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}


void SupplierDlg::on_delBtn_clicked()   //删除选中行
{
    int row = ui->tableView->currentIndex().row();
    QSqlRecord record = supplierModel->record(row);
    int id = record.value(Supplier_Id).toInt();
    QSqlQuery query;
    if(query.exec(tr("delete from supplierTb where id = %1").arg(id)))
    {
        supplierModel->setFilter("");
        supplierModel->select();
    }
}

void SupplierDlg::on_addBtn_clicked()           //添加供应商
{
    AddSupplierDlg dlg;
    if(dlg.exec() == QDialog::Accepted)
    {
        supplierModel->setFilter("");
        supplierModel->select();
        ui->tableView->selectRow(supplierModel->rowCount() - 1);
    }
}

void SupplierDlg::copyTheNameSlot()     //右键复制供应商名字
{
    QString name = supplierModel->index(ui->tableView->currentIndex().row(),1).data().toString();
    QApplication::clipboard()->setText(name);
}

void SupplierDlg::copyTheNumberSlot()       //右键复制供应商电话
{
    QString number = supplierModel->index(ui->tableView->currentIndex().row(),3).data().toString();
    QApplication::clipboard()->setText(number);
}
