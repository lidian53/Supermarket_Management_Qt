#include "addpurchasedlg.h"
#include "ui_addpurchasedlg.h"
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <QMessageBox>
#include <QSqlQuery>
#include <QCompleter>
#include "choicegoodsdlg.h"
#include <QSqlQueryModel>
#include "goodsnumdelegate.h"
#include <QSqlError>

AddPurchaseDlg::AddPurchaseDlg(QString handlerName, QWidget *parent) :
    QDialog(parent),handler(handlerName),
    ui(new Ui::AddPurchaseDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    initialisePage();
    createComploeter();
}

void AddPurchaseDlg::initialisePage()
{
    ui->tableWidget->setItemDelegate(new GoodsNumDelegate(2));      //设置第三列数量的委托
    connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(updateModel(QTableWidgetItem*)));
    ui->addBtn->setDefault(true);
    ui->goodsIdLineEdit->installEventFilter(this);
    ui->spinBox->installEventFilter(this);
    ui->goodsIdLineEdit->setFocus();
    ui->tableWidget->setColumnWidth(0,150);
    ui->tableWidget->setColumnWidth(1,150);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->spinBox->setMinimum(1);
    supplierModel = new QSqlQueryModel;
    supplierModel->setQuery(tr("select name from supplierTb"));
    ui->supplierComboBox->setModel(supplierModel);
    //生成单号
    QString str = QDateTime::currentDateTime().toString("yyyyMMddhhmmss")
            + QString::number(qrand() % 899 + 100);
    ui->purchaseIdLineEdit->setText(str);
    purchaseId = str;
    ui->handlerLabel->setText(QString("操作员：") + handler);       //
}

void AddPurchaseDlg::createComploeter() //创建文本框自动补全
{
    QStringList list;
    QSqlQuery query;
    query.exec(tr("select num from goodsNameTb"));
    while(query.next())
    {
        list << query.value(0).toString();
    }
    completer = new QCompleter(list,this);
    ui->goodsIdLineEdit->setCompleter(completer);
}

void AddPurchaseDlg::selectGoods()
{
    ChoiceGoodsDlg dlg;
    QString num;
    if(dlg.exec() == QDialog::Accepted)
    {
        num = dlg.getChoiceGoodsNum();
        ui->goodsIdLineEdit->setText(num);
    }
}

void AddPurchaseDlg::addToTable(const QString &num)     //根据商品编号，将商品添加到表格中
{
    QSqlQuery query;
    //先检查是否已经有了该商品的信息
    for(int i = 0;i < ui->tableWidget->rowCount();++i)
    {
        if(ui->tableWidget->item(i,0)->text() == num)
        {
            int count = ui->tableWidget->item(i,2)->text().toInt();
            ui->tableWidget->item(i,2)->setText(QString::number(count + ui->spinBox->value()));
            updatePrice();  //更新总价
            return;
        }
    }
    query.exec(tr("select name,bid from goodsNameTb where num = '%1'").arg(num));
    if(query.next())
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem(num);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable );
        ui->tableWidget->setItem(row,0,item);
        QTableWidgetItem *item1 = new QTableWidgetItem(query.value(0).toString());
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable );
        ui->tableWidget->setItem(row,1,item1);
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(ui->spinBox->value()));
        ui->tableWidget->setItem(row,2,item2);
        QTableWidgetItem *item3 = new QTableWidgetItem(query.value(1).toString());
        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable );
        ui->tableWidget->setItem(row,3,item3);
    }
}

void AddPurchaseDlg::updatePrice()      //遍历整个表，计算总价
{
    QSqlQuery query;
    double totalPrice = 0.0;
    for(int i = 0;i < ui->tableWidget->rowCount();i++)
    {
        QString id = ui->tableWidget->item(i,0)->text();        //商品编号
        int count = ui->tableWidget->item(i,2)->text().toInt(); //数量
        query.exec(tr("select bid from goodsNameTb where num = '%1'").arg(id));
        query.next();
        double price =  query.value(0).toDouble();
        totalPrice += count * price;
    }
    ui->totalPriceLineEdit->setText(QString::number(totalPrice));
}

AddPurchaseDlg::~AddPurchaseDlg()
{
    delete ui;
}

bool AddPurchaseDlg::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->spinBox || target == ui->goodsIdLineEdit)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if(target == ui->goodsIdLineEdit && ui->goodsIdLineEdit->text().isEmpty())
                {
                    selectGoods();
                }
                else
                {
                    focusNextChild();
                }
                return true;

            }
        }
    }
    return QDialog::eventFilter(target,event);
}

void AddPurchaseDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 70)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void AddPurchaseDlg::paintEvent(QPaintEvent *event)
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

void AddPurchaseDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void AddPurchaseDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}


void AddPurchaseDlg::on_addBtn_clicked()
{
    if(ui->goodsIdLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("请输入商品名"));
        ui->goodsIdLineEdit->setFocus();
        return;
    }
    addToTable(ui->goodsIdLineEdit->text());
    ui->goodsIdLineEdit->clear();
    ui->spinBox->setValue(1);
    ui->goodsIdLineEdit->setFocus();
 //   updatePrice();
}

void AddPurchaseDlg::on_delBtn_clicked()        //删除选中的商品
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)
    {
        QMessageBox::warning(this,tr("警告"),tr("请选择需要删除的商品"));
        return;
    }
    ui->tableWidget->removeRow(row);
    updatePrice();
}

void AddPurchaseDlg::on_purchaseIdLineEdit_editingFinished()
{
    if(ui->purchaseIdLineEdit->text().isEmpty())
    {
        ui->purchaseIdLineEdit->setText(purchaseId);
        return;
    }
    if(ui->purchaseIdLineEdit->text() != purchaseId)        //说明已经改变
    {
        QString text = ui->purchaseIdLineEdit->text();
        QSqlQuery query;
        query.exec(tr("select id from purchaseTb"));
        while(query.next())
        {
            if(query.value(0).toString() == text)
            {
                QMessageBox::warning(this,tr("警告"),tr("抱歉，该单号已经存在，单号被重置!"));
                ui->purchaseIdLineEdit->setText(purchaseId);
                break;
            }
        }
        return;
    }

}

void AddPurchaseDlg::updateModel(QTableWidgetItem *item)        //如果商品数量发生改变，就重新计算价格
{
    if(item->column() == 2)
    {
        updatePrice();
    }
}

void AddPurchaseDlg::on_okBtn_clicked()         //提交这个订单
{
    if(ui->tableWidget->rowCount() == 0)
    {
        QMessageBox::warning(this,tr("警告"),tr("进货清单为空，请添加商品!"));
        return;
    }
    QSqlQuery query;
    query.prepare(tr("insert into purchaseTb values(:id,:dateTime,:supplierId,:totalPrice,"
                     ":handler,:note)"));
    query.bindValue(":id",purchaseId);      //订单号
    query.bindValue(":dateTime",ui->dateEdit->date().toString("yyyy-MM-dd"));      //时间
    QSqlQuery idQuery;
    idQuery.exec(tr("select id from supplierTb where name = '%1'").arg(ui->supplierComboBox->currentText()));
    idQuery.next();
    query.bindValue(":supplierId",idQuery.value(0).toInt());
    query.bindValue(":totalPrice",ui->totalPriceLineEdit->text().toDouble());
    query.bindValue(":handler",handler);
    query.bindValue(":note",ui->textEdit->toPlainText());
    if(! query.exec())
    {
        QMessageBox::warning(this,tr("this"),tr("添加失败,%1").arg(query.lastError().text()));
        return;
    }
    else
        QMessageBox::information(this,tr("ta"),tr("添加成功"));
    //添加商品明细
    int id;
    QSqlQuery detailIdQuery;
    detailIdQuery.exec(tr("select id from purchaseDetailTb"));      //取出最后的id
    if(detailIdQuery.last())        //如果有记录
    {
        id = detailIdQuery.value(0).toInt();
    }
    else
    {
        id = 0;
    }

    for(int i = 0;i < ui->tableWidget->rowCount();++i)      //遍历整个表格
    {
        query.exec(tr("insert into purchaseDetailTb values(:id,:purchaseId,:goodsId,:count)"));
        query.bindValue(":id",++id);        //主键id递增
        query.bindValue(":purchaseId",ui->purchaseIdLineEdit->text());
        query.bindValue(":goodsId",ui->tableWidget->item(i,0)->text());
        query.bindValue(":count",ui->tableWidget->item(i,2)->text().toInt());
        query.exec();
    }
    this->accept();     //关闭窗口
}
