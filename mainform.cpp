#include "mainform.h"
#include "ui_mainform.h"
#include <QMouseEvent>
#include <QDate>
#include <QTableView>
#include <QStringListModel>
#include <QListView>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSqlError>
#include "showinforlabel.h"
#include <QCompleter>
#include <QTimer>
#include "goodsnumdelegate.h"
#include <QPainter>
#include <qmath.h>
#include "collectdlg.h"
#include "choicegoodsdlg.h"
#include <QFileDialog>
#include <QAxObject>
#include <QKeyEvent>
#include "maskwidget.h"
#include "addpurchasedlg.h"
#include <QClipboard>
#include "addsupplierdlg.h"
#include "supplierdlg.h"
#include "navmodel.h"
#include "navdelegate.h"
#include "histogramview.h"
#include "adduserdlg.h"
#include <QVBoxLayout>
#include "modifyuserdlg.h"
#include "addgoodtypedlg.h"
#include <QInputDialog>

MainForm::MainForm(QString username, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    userName = username;

    ui->setupUi(this);
    goodsNameModel_M = 0;
    UserManagementModel_U = 0;
    maskWidget = 0;
    goodsListWidget = 0;
    billModel_B = 0;
    purchaseModel_P = 0;
    histogramView = 0;

    model = new NavModel(this);
    //根据不同的用户进行不同页面的显示，非管理员用户只能进入收银台的界面
    if(userName == tr("admin"))
    {
        model->ReadDataFromConfig(":/config/config/configForAdmin.xml");
    }
    else
    {
        model->ReadDataFromConfig(":/config/config/configForGeneral.xml");
    }


    NavDelegate* delegate = new NavDelegate(this);
    ui->listView->setModel(model);
    ui->listView->setItemDelegate(delegate);
    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)),
            model, SLOT(Collapse(const QModelIndex&)));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),model,SLOT(clickIndex(QModelIndex)));
    connect(model,SIGNAL(clickItem(QString)),this,SLOT(clickInfor(QString)));
 //   userName = "hxb";
    curTimeId = startTimer(1000);       //启动定时器，1秒一次
    showCurrentTime();  //显示右下角的时间
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);//任务栏使程序最小化
    createPage();
  //  ui->barcodeLineEdit_S->setFocus();
 //   ui->barcodeLineEdit_S->installEventFilter(this);

    setSellCount();     //获取今天的销售数量，做单号用
/*    QTableView *view = new QTableView;
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("sellTb");
    model->select();
    view->setModel(model);
    view->show(); */
 //   showGoodsInforSlot();

    ui->stackedWidget->setCurrentIndex(6);  //显示为首页
    this->showName();       //为底部显示用户名

}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::setUserLastLoginTime()   //将当前用户最后登录时间存入到数据库中
{
    QSqlDatabase::database().transaction();
    QString curDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QSqlQuery query;
    QString str = tr("UPDATE userTb"
                     " SET lastLogTime='%1' WHERE username='%2'")
                       .arg(curDateTime).arg(this->userName);
    if(query.exec(str))    //更新成功
    {
        QSqlDatabase::database().commit();
    }
    else
    {
        QSqlDatabase::database().rollback();
        QMessageBox::warning(this,tr("警告"),tr("登录时间更新失败!\n错误原因：")
                             .arg(query.lastError().text()),QMessageBox::Ok);
    }
}


bool MainForm::eventFilter(QObject *target, QEvent *event)  //事件过滤器，按enter跳至下一个控件
{
    /*  if(target == ui->barcodeLineEdit_S)
    {
        if(event->type() == QEvent::KeyPress)
        {
            qDebug() << "enter";
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            qDebug() << keyEvent->key();
            if(keyEvent->key() == Qt::Key_Enter)
            {
                qDebug() << "key";
                return true;
            }
        }
    } */

    if(target == ui->goodsIdLineEdit_M || target == ui->goodsNameLineEdit_M || target == ui->stockSpinBox_M
            || target == ui->bidSpinBox_M || target == ui->retailSpinBox_M || target == ui->formatLineEdit_M
            || target == ui->unitLineEdit_M || target == ui->noteTextEdit_M)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Return)
            {
                focusNextChild();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}

void MainForm::showName()
{
    //设置底部“当前用户”label
    ui->showUserLabel->setText("当前用户：" + this->userName);
    //设置当前登录的用户的最后登录时间存入到数据库中
    setUserLastLoginTime();
}

void MainForm::mousePressEvent(QMouseEvent *event)      //拉动窗体
{
    if((event->button() == Qt::LeftButton) && event->pos().y() < 110)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;          //拉动窗体
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
    QWidget::mousePressEvent(event);
}

void MainForm::mouseMoveEvent(QMouseEvent *event)       //拉动窗体
{
    //拖动条件：左键 、 y值小于110（只能拖动标题栏部分）
    if((event->buttons() & Qt::LeftButton) && event->pos().y() < 110 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void MainForm::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
    QWidget::mouseReleaseEvent(event);
}


void MainForm::timerEvent(QTimerEvent *event)       //定时器事件
{
    if(event->timerId() == curTimeId)       //判断是否为相应的定时器
    {
        showCurrentTime();      //显示时间
    }
}

void MainForm::paintEvent(QPaintEvent *event)
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

void MainForm::keyPressEvent(QKeyEvent *event)  //处理快捷键
{
    if(event->key() == Qt::Key_F1)
    {
        if(ui->stackedWidget->currentIndex() == 2)
        {
            on_collectionsBtn_S_clicked();
            return;
        }
    }
    else if(event->key() == Qt::Key_F2) //锁屏
    {
        QSqlQuery query;
        query.exec(tr("select password from userTb where username = '%1'").arg(userName));
        query.next();
        if(! maskWidget)
        {
            maskWidget = new MaskWidget(query.value(0).toString(),this);
        }
        maskWidget->resize(this->size() - QSize(20,20));
        maskWidget->move(10,10);
     //   w->move(this->pos() + QPoint(10,10));
        maskWidget->show();
    }
    else if(event->key() == Qt::Key_F3) // 跳转到首页
    {
        ui->stackedWidget->setCurrentIndex(6);
    }
}

void MainForm::showEvent(QShowEvent *event) //窗体显示时，重置最小化按钮，修复bug
{

}

void MainForm::showCurrentTime()    //在窗体右下角显示时间
{
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    int n = date.dayOfWeek();
    QString week;
    switch(n)
    {
    case 1:
        week = tr("星期一");
        break;
    case 2:
        week = tr("星期二");
        break;
    case 3:
        week = tr("星期三");
        break;
    case 4:
        week = tr("星期四");
        break;
    case 5:
        week = tr("星期五");
        break;
    case 6:
        week = tr("星期六");
        break;
    case 7:
        week = tr("星期天");
        break;
    }
    QString dateAndTime = date.toString("yyyy-MM-dd") + QString(" ")
            + time.toString("hh:mm:ss") + QString(" ") + week;
    //时间格式为年-月-日 小时：分钟：秒 星期
    ui->curTimeLabel->setText(dateAndTime);
}

void MainForm::installWidgetEventFilter()           //为商品添加界面的控件安装事件过滤器
{
    ui->goodsIdLineEdit_M->installEventFilter(this);
    ui->goodsNameLineEdit_M->installEventFilter(this);
    ui->stockSpinBox_M->installEventFilter(this);
    ui->bidSpinBox_M->installEventFilter(this);
    ui->retailSpinBox_M->installEventFilter(this);
    ui->formatLineEdit_M->installEventFilter(this);
    ui->unitLineEdit_M->installEventFilter(this);
    ui->noteTextEdit_M->installEventFilter(this);
}

void MainForm::createPage()
{
    if(! goodsNameModel_M)
    {
        goodsNameModel_M = new QSqlRelationalTableModel;
        goodsNameModel_M->setTable(tr("goodsNameTb"));
        goodsNameModel_M->setRelation(GoodsName_TypeId,QSqlRelation("goodsTypeTb","id","typename"));
        goodsNameModel_M->select();
        goodsNameModel_M->setHeaderData(GoodsName_Id,Qt::Horizontal,tr("商品编号"));
        goodsNameModel_M->setHeaderData(GoodsName_TypeId,Qt::Horizontal,tr("商品类别"));
        goodsNameModel_M->setHeaderData(GoodsName_Name,Qt::Horizontal,tr("商品名"));
        goodsNameModel_M->setHeaderData(GoodsName_Stock,Qt::Horizontal,tr("库存"));
        goodsNameModel_M->setHeaderData(GoodsName_Bid,Qt::Horizontal,tr("售价"));
        goodsNameModel_M->setHeaderData(GoodsName_Retail,Qt::Horizontal,tr("零售价"));
        goodsNameModel_M->setHeaderData(GoodsName_Format,Qt::Horizontal,tr("规格"));
        goodsNameModel_M->setHeaderData(GoodsName_Unit,Qt::Horizontal,tr("单位"));
        goodsNameModel_M->setHeaderData(GoodsName_Note,Qt::Horizontal,tr("备注"));
        ui->goodsNameTableView_M->setModel(goodsNameModel_M);
        ui->goodsNameTableView_M->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->goodsNameTableView_M->resizeColumnsToContents();
        ui->goodsNameTableView_M->horizontalHeader()->setStretchLastSection(true);
        comboBoxmodel_M = new QSqlQueryModel;
        installWidgetEventFilter(); //安装控件的事件过滤器
    }
    comboBoxmodel_M->setQuery(QString("select typename from goodsTypeTb"));
    ui->goodsTypeComboBox_M->setModel(comboBoxmodel_M);
}

void MainForm::showGoodsListWidget()        //选择商品列表
{
    goodsListWidget->show();
}

void MainForm::produceSellID()          //生成单号
{
    QString date = QDate::currentDate().toString("yyyyMMdd");
    QString str = date + QString("_") + QString::number(countOfSell);
    ui->sellIdLineEdit_S->setText(str);
    sellIdStr = str;
}

void MainForm::setSellCount()           //获取今天的单数
{
    QSqlQuery query;
    query.exec(tr("select dateTime from sellListTb"));  //遍历所有日期
    if(query.last())        //查看最后一天的销售的日期
    {
        if(query.value(0).toDate() == QDate::currentDate())     //如果是今天，就取出今天的单子数量
        {
            int count = 0;
            QString date = QDate::currentDate().toString("yyyy-MM-dd");
            query.exec(tr("select todayCount from sellListTb where dateTime = '%1'").arg(date));
            if(query.last())
            {
                count = query.value(0).toInt();     //取出今天的销售单数
                countOfSell = count;        //今天的销售数量
                return;
            }
        }
        else
        {
            countOfSell = 0;            //重置今天的数量为0，根据销售数量生成单号
        }
    }
    else
        countOfSell = 0;
}

void MainForm::showGoodsInforSlot()               //商品一览
{
    ui->stackedWidget->setCurrentIndex(1);
    setWidgetStatus_M(false);       //设置所有控件为不可编辑
    //将商品类别添加到comboBox中
    ui->addGoodsTypeCombox_M->clear();  //先清空
    QSqlQuery query;
    query.exec("select typename from goodsTypeTb");
    query.next();       //跳过第一条记录，“查看所有商品”;
    while(query.next())
    {
        ui->addGoodsTypeCombox_M->addItem(query.value(0).toString());
    }

    on_goodsTypeComboBox_M_currentIndexChanged(QString(""));
}

void MainForm::showSellPageSlot()             //单击收银台按钮
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->barcodeLineEdit_S->setFocus();
    QStringList listOfBarCode;
    QSqlQuery query;
    query.exec(tr("select * from goodsNameTb"));
    while(query.next())
    {
        listOfBarCode << query.value(0).toString();
    }
    QCompleter *barCodeCompleter = new QCompleter(listOfBarCode,this);
    connect(barCodeCompleter,SIGNAL(highlighted(QString)),this,SLOT(completerChange(QString)));
    connect(barCodeCompleter,SIGNAL(activated(QString)),this,SLOT(destoryCompleter()));
    ui->barcodeLineEdit_S->setCompleter(barCodeCompleter);
    if(! goodsListWidget)               //当在条码框直接回车弹出选择商品列表
    {
        goodsListWidget = new QTableView;
        goodsListWidget->resize(400,300);
        goodsListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        goodsListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        connect(goodsListWidget,SIGNAL(activated(QModelIndex)),this,SLOT(goodsListDoubleClick(QModelIndex)));
        connect(goodsListWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(goodsListDoubleClick(QModelIndex)));
        goodsListWidget->setModel(goodsNameModel_M);
        //设置委托  --出售列表第七列数量的委托
        ui->sellList_S->setItemDelegate(new GoodsNumDelegate(7));
        connect(ui->sellList_S,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(updateModel(QTableWidgetItem*)));
        delAction = new QAction(tr("删除该商品(&D)"),this);
        addAction = new QAction(tr("数量加一(&A)"),this);
        reduceAction = new QAction(tr("数量减一(&R)"),this);
        ui->sellList_S->addAction(delAction);
        connect(delAction,SIGNAL(triggered()),this,SLOT(deleteSellListSlot()));
        ui->sellList_S->addAction(addAction);
        ui->sellList_S->addAction(reduceAction);
        ui->sellList_S->setContextMenuPolicy(Qt::ActionsContextMenu);
        sumOfPrice = 0.0;
        //生成单号
        produceSellID();

        //关联单号LineEdit的回车键
        connect(ui->barcodeLineEdit_S,SIGNAL(returnPressed()),
                this,SLOT(destoryCompleter()));
    }
}

void MainForm::showSallStatisticsSlot()
{
    ui->stackedWidget->setCurrentIndex(5);
    if(! histogramView)
    {
        histogramView = new HistogramView(this);
        histogramView->setModel(billModel_B);
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(histogramView);
        layout->setContentsMargins(10,10,40,20);      //设置边缘距离
        ui->sallStatisticsPage->setLayout(layout);
    }
}

void MainForm::showUserManagementSlot()     //用户管理界面
{
    ui->stackedWidget->setCurrentIndex(7);  //跳转到用户界面
    if(! UserManagementModel_U)
    {
        UserManagementModel_U = new QSqlTableModel;
    }
    UserManagementModel_U->setTable(tr("UserTb"));
    UserManagementModel_U->select();    //查询所有数据

    UserManagementModel_U->setHeaderData(User_Name,Qt::Horizontal,tr("用户名"));
    UserManagementModel_U->setHeaderData(User_Password,Qt::Horizontal,tr("密码"));
    UserManagementModel_U->setHeaderData(User_JobNum,Qt::Horizontal,tr("工作号"));
    UserManagementModel_U->setHeaderData(User_Sex,Qt::Horizontal,tr("性别"));
    UserManagementModel_U->setHeaderData(User_IDCard,Qt::Horizontal,tr("身份证号"));
    UserManagementModel_U->setHeaderData(User_LastLoginTime,Qt::Horizontal,tr("上次登录时间"));

    ui->UserTableView_U->setModel(UserManagementModel_U);

    ui->UserTableView_U->setSelectionBehavior(QAbstractItemView::SelectRows);
   // ui->UserTableView_M->resizeColumnsToContents();
    ui->UserTableView_U->setColumnWidth(User_IDCard,200);

    ui->UserTableView_U->horizontalHeader()->setStretchLastSection(true);
}

void MainForm::on_barcodeLineEdit_S_returnPressed()     //当条码输入框为空时，弹出对话框选择商品
{
  //  destoryCompleter();
    if(ui->barcodeLineEdit_S->text().isEmpty())     //如果文本框内容为空
    {
       // showGoodsListWidget();
        ChoiceGoodsDlg dlg;         //创建选择商品的对话框
        if(dlg.exec() == QDialog::Accepted)
        {
            ui->barcodeLineEdit_S->setText(dlg.getChoiceGoodsNum());    //设置条码框内容
            destoryCompleter();     //进行添加商品
        }
    }
    ui->barcodeLineEdit_S->setFocus();
}

void MainForm::completerChange(const QString &arg)
{
    QSqlQuery query;
    query.exec(tr("select * from goodsNameTb where num = '%1'").arg(arg));
    if(query.next())
    {
        QString str = QString("<b>商品名：</b>%1 <b>库存：</b>%2 <b>单价：</b>%3")
                .arg(query.value(GoodsName_Name).toString())
                     .arg(query.value(GoodsName_Stock).toString())
                          .arg(query.value(GoodsName_Retail).toString());
        ui->goodsDetailLabel_S->setText(str);

    }
}

void MainForm::destoryCompleter()           //自动补全按下回车触发
{
    QString arg1 = ui->barcodeLineEdit_S->text();
    ui->barcodeLineEdit_S->clear();
    if(arg1.length() == 9)      //条码固定9位数字，一旦有了9位数字就添加商品
    {
        //检索已经添加的商品是否重复
        QStringList list ;
        for(int i = 0;i < ui->sellList_S->rowCount();i++)
            list << ui->sellList_S->item(i,0)->text();

        if(list.contains(arg1))     //如果已经添加了商品
        {
            int index = list.indexOf(arg1);
            int num = ui->sellList_S->item(index,7)->text().toInt() + 1;
            ui->sellList_S->item(index,7)->setText(QString::number(num));
        }
        else
        {
            QSqlQuery query;
            query.exec(tr("select *from goodsNameTb where num = '%1'").arg(arg1));

            if(query.next())        //如果有该商品
            {
                int num = ui->sellList_S->rowCount();
                ui->sellList_S->insertRow(num);
                QTableWidgetItem *item0 = new QTableWidgetItem(arg1);   //条形码
                item0->setFlags(item0->flags() & ~Qt::ItemIsEditable );
                ui->sellList_S->setItem(num,0,item0);
                QTableWidgetItem *item1 = new QTableWidgetItem(query.value(GoodsName_Name).toString());
                ui->sellList_S->setItem(num,1,item1);
                item1->setFlags(item1->flags() & ~Qt::ItemIsEditable );
                QTableWidgetItem *item3 = new QTableWidgetItem(query.value(GoodsName_Format).toString());
                ui->sellList_S->setItem(num,3,item3);
                item3->setFlags(item3->flags() & ~Qt::ItemIsEditable );
                QTableWidgetItem *item4 = new QTableWidgetItem(query.value(GoodsName_Stock).toString());
                ui->sellList_S->setItem(num,4,item4);
                item4->setFlags(item4->flags() & ~Qt::ItemIsEditable );
                QTableWidgetItem *item5 = new QTableWidgetItem(query.value(GoodsName_Unit).toString());
                ui->sellList_S->setItem(num,5,item5);
                item5->setFlags(item5->flags() & ~Qt::ItemIsEditable );
                QTableWidgetItem *item6 = new QTableWidgetItem(query.value(GoodsName_Retail).toString());
                ui->sellList_S->setItem(num,6,item6);
                item6->setFlags(item6->flags() & ~Qt::ItemIsEditable );
                QTableWidgetItem *item7 = new QTableWidgetItem(tr("1"));
                ui->sellList_S->setItem(num,7,item7);          
            }
        }
        updateSellPrice();          //更新总金额
    }
    QTimer::singleShot(0,ui->barcodeLineEdit_S,SLOT(clear()));
    ui->barcodeLineEdit_S->setFocus();
    ui->goodsDetailLabel_S->clear();
}

void MainForm::goodsListDoubleClick(QModelIndex index)
{
    ui->barcodeLineEdit_S->setText(goodsNameModel_M->index(index.row(),0).data(Qt::DisplayRole).toString());
    destoryCompleter();
}

void MainForm::deleteSellListSlot()         //删除出售清单中的数据
{
    ui->sellList_S->removeRow(ui->sellList_S->currentRow());
    updateSellPrice();
    ui->barcodeLineEdit_S->setFocus();
}

void MainForm::updateSellPrice()            //当更新了出售列表后，重新计算价格以及数量
{
    sumOfPrice = 0.0;
    int count = 0;
    for(int i = 0;i < ui->sellList_S->rowCount();i++)
    {

        double sum = ui->sellList_S->item(i,6)->text().toDouble();
        double price = ui->sellList_S->item(i,7)->text().toDouble();
        //设置第八列，总金额的item
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(sum * price));
        ui->sellList_S->setItem(i,8,item);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable );
        count += ui->sellList_S->item(i,7)->text().toInt();
        sumOfPrice += sum * price;
    }
    ui->sumOfMoneyLabel_S->setText(QString::number(sumOfPrice));
    ui->sellCountLabel_S->setText(QString::number(count));
}

void MainForm::updateModel(QTableWidgetItem *item)  //当购买商品数量发生改变时触发
{
    if(item->column() == 7)
        updateSellPrice();
}

void MainForm::on_delSellGoodsBtn_S_clicked()           //购买界面，删除按钮
{
    this->deleteSellListSlot(); //
}

void MainForm::on_collectionsBtn_S_clicked()        //购买界面，购买按钮
{
    if(ui->sellList_S->rowCount() == 0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("购买清单为空，请先添加商品！"));
        return;
    }
    collectDlg dlg(sumOfPrice);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString dateAndTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        double factPay = dlg.getPayPrice(); //获取实付
        int count = ui->sellCountLabel_S->text().toInt();
        QSqlQuery query;
        query.prepare(tr("insert into sellTb values(:sellId,:sellMan,:date,:factPay,:collect,:count)"));
        query.bindValue(":sellId",sellIdStr);
        query.bindValue(":sellMan",userName);
        query.bindValue(":date",dateAndTime);
        query.bindValue(":factPay",factPay);
        query.bindValue(":collect",sumOfPrice);       //应付
        query.bindValue(":count",count);
        query.exec();
    }
    else
        return;
    int serialNum;
    QSqlQuery query;
    query.exec(tr("select serialNum from sellListTb"));
    if(query.last())
    {
        serialNum = query.value(0).toInt();     //获取最后一条数据的id
    }
    QString goodsId;            //商品id
    int count;                      //数量
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    countOfSell++;          //今天的销售数量加1
    for(int row = 0;row < ui->sellList_S->rowCount();row++)
    {
        goodsId = ui->sellList_S->item(row,0)->text();
        count = ui->sellList_S->item(row,7)->text().toInt();
        query.prepare(tr("insert into sellListTb "
                         "values(:num,:sellId,:sellCount,:goodsId,:count,:date)"));
        query.bindValue(":num",++serialNum);
        query.bindValue(":sellId",sellIdStr);         //单号
        query.bindValue(":sellCount",countOfSell);
        query.bindValue(":goodsId",goodsId);
        query.bindValue(":count",count);
        query.bindValue(":date",date);
        query.exec();
      //  query.exec(tr("insert into sellListTb "
                //      "values(%1,'%2','%3',%4,'%5')").arg(++serialNum).arg(sellIdStr)
                 //  .arg(goodsId).arg(count).arg(date));
    }
    produceSellID();        //重新生成
    ui->sellList_S->setRowCount(0);
    ui->barcodeLineEdit_S->setFocus();
    updateSellPrice();      //重置价格
}

void MainForm::on_sellIdLineEdit_S_lostFocus()          //单号文本框失去焦点，单号发生改变(可能也没改变)
{
    QString text = ui->sellIdLineEdit_S->text();
   if(text.isEmpty())       //单号不得为空
   {
      QMessageBox::warning(this,tr("警告"),tr("单号不得为空，单号重置"));
      produceSellID();        //重置单号
      return;
   }
   if(text != sellIdStr)        //如果和当前的单号不相同,说明已经改变
   {
       if(text.length() > 15)       //长度不能超过15
       {
           QMessageBox::warning(this,tr("警告"),tr("单号不得超过15个字符"));
           return;
       }
       QSqlQuery query;
       //查询是否有这个单号
       query.exec(tr("select *from sellListTb where sellId = '%1'").arg(text));
       if(query.next())
       {
           QMessageBox::warning(this,tr("警告"),tr("该单号已经存在，单号重置"));
           produceSellID();
           return;
       }
       sellIdStr = text;        //更新单号
   }
}

void MainForm::on_sellIdLineEdit_S_returnPressed()      //单号文本框按下回车时，取消它的焦点，从而调用lostFocus()方法
{
    ui->sellIdLineEdit_S->clearFocus();
}

void MainForm::showHistoryBillSlot()      //查看历史账单
{
    ui->stackedWidget->setCurrentIndex(3);
    QSqlQuery query;
    query.exec(tr("select sellDateTime from sellTb"));  //查询售货单第一条记录
    if(query.next())
    {
        QDateTime date = query.value(0).toDateTime();       //获取第一条数据的时间
        ui->startDateTimeEdit_B->setDateTime(date);         //设置查询时间为第一条数据时间
    }
    else
    {
        ui->startDateTimeEdit_B->setDateTime(QDateTime::currentDateTime());
    }
    ui->endDateTimeEdit_B->setDateTime(QDateTime::currentDateTime());
    //添加操作员到comboBox中
    ui->sellManComboBox_B->clear();
    ui->sellManComboBox_B->addItem(tr("所有记录"));     //默认将所欲记录都显示
    query.exec(tr("select username from userTb"));
    while(query.next())
    {
        ui->sellManComboBox_B->addItem(query.value(0).toString());
    }
//    QMessageBox::warning(this,tr("this"),QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    updateBillPageSlot();   //更新账单
}

void MainForm::updateBillPageSlot()     //更新账单
{
    QString startDatetime = ui->startDateTimeEdit_B->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString endDatetime = ui->endDateTimeEdit_B->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sellManName = ui->sellManComboBox_B->currentText();
    if(! billModel_B)
    {
        billModel_B = new QSqlTableModel;
        billModel_B->setTable("sellTb");
        billModel_B->select();
 /*       connect(ui->sellListTableView_B->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex))
                ,this,SLOT(on_sellListTableView_B_clicked(QModelIndex))); */
        ui->sellListTableView_B->setModel(billModel_B);
        ui->sellListTableView_B->horizontalHeader()->setStretchLastSection(true);
        ui->sellListTableView_B->setColumnWidth(Sell_DateTime,200);
        billModel_B->setHeaderData(Sell_Id,Qt::Horizontal,tr("单号"));
        billModel_B->setHeaderData(Sell_Man,Qt::Horizontal,tr("操作员"));
        billModel_B->setHeaderData(Sell_DateTime,Qt::Horizontal,tr("时间"));
        billModel_B->setHeaderData(Sell_FactPrice,Qt::Horizontal,tr("实际付款"));
        billModel_B->setHeaderData(Sell_CollectPrice,Qt::Horizontal,tr("应收"));
        billModel_B->setHeaderData(Sell_Count,Qt::Horizontal,tr("商品数量"));

        billListModel_B = new QSqlRelationalTableModel(this);
        billListModel_B->setTable("sellListTb");
        billListModel_B->setRelation(SellList_GoodsId,QSqlRelation("goodsNameTb","num","name"));
        billListModel_B->setHeaderData(SellList_Id,Qt::Horizontal,tr("单号"));
        billListModel_B->setHeaderData(SellList_GoodsId,Qt::Horizontal,tr("商品名"));
        billListModel_B->setHeaderData(SellList_Count,Qt::Horizontal,tr("数量"));
        billListModel_B->setHeaderData(SellList_DateTime,Qt::Horizontal,tr("日期"));
        //关系表
        billListModel_B->select();
        ui->sellDetailTableWidget_B->setModel(billListModel_B);
        ui->sellDetailTableWidget_B->setColumnHidden(SellList_Num,true);
        ui->sellDetailTableWidget_B->setColumnHidden(SellList_TodayCount,true);
        ui->sellDetailTableWidget_B->horizontalHeader()->setStretchLastSection(true);
        ui->sellDetailTableWidget_B->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->sellDetailTableWidget_B->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->sellDetailTableWidget_B->setSelectionMode(QAbstractItemView::SingleSelection);


    }
    if(sellManName == tr("所有记录"))
    {
        billModel_B->setFilter(tr("sellDateTime >= '%1' and sellDateTime <= '%2'")
                               .arg(startDatetime).arg(endDatetime));
    }
    else
    {
        billModel_B->setFilter(tr("sellDateTime >= '%1' and sellDateTime <= '%2' and sellMan = '%3'")
                               .arg(startDatetime).arg(endDatetime).arg(sellManName));
    }
    billModel_B->select();
    if(billModel_B->rowCount() > 0)
    {
        ui->sellListTableView_B->selectRow(0);          //如果有销售记录的，就选择第一条记录
        on_sellListTableView_B_clicked(QModelIndex());      //相当于点击了第一条记录，从而显示详细信息
    }
}

void MainForm::on_startDateTimeEdit_B_dateTimeChanged(const QDateTime &date)
{
    ui->endDateTimeEdit_B->setMinimumDateTime(date);
    updateBillPageSlot();
}

void MainForm::on_endDateTimeEdit_B_dateTimeChanged(const QDateTime &date)
{
    ui->startDateTimeEdit_B->setMaximumDateTime(date);
    updateBillPageSlot();
}

void MainForm::on_sellListTableView_B_clicked(const QModelIndex &index)
{
    int row = ui->sellListTableView_B->currentIndex().row();
    QModelIndex temp = billModel_B->index(row,0);
    if(temp.isValid())
    {
        QString str = temp.data(Qt::DisplayRole).toString();
        billListModel_B->setFilter(tr("sellId = '%1'").arg(str));
        billListModel_B->select();
    }
}

void MainForm::on_searchForIdLineEdit_B_textChanged(const QString &arg1)    //历史账单单号查询
{
    billModel_B->setFilter(tr("sellId like '%1%'").arg(arg1));  //模糊查询
    billModel_B->select();
    if(billModel_B->rowCount() != 0)
    {
       ui->sellListTableView_B->selectRow(0);
       this->on_sellListTableView_B_clicked(QModelIndex());
    }
}

void MainForm::on_exportToExcelBtn_B_clicked()          //将历史账单导出为excel
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("导出"),tr("."),tr("Microsoft Office(*.xls)"));
    if(fileName.isEmpty())
    {
        return;
    }
}

void MainForm::on_clearSellListBtn_S_clicked()      //收银台界面的清空
{
    if(ui->sellList_S->rowCount() == 0)         //查看商品数量是否为0
        return;
    for(int i = ui->sellList_S->rowCount();i >= 0;i--)  //从后往前遍历，清空所有数据
    {
        ui->sellList_S->removeRow(i);
    }
//    while(ui->sellList_S->rowCount() >0)
  //      ui->sellList_S->removeRow(0);
    updateSellPrice();          //更新金额
    ui->barcodeLineEdit_S->setFocus();
}

void MainForm::on_delBillBtn_B_clicked()            //删除账单中的某条记录
{
    int row = ui->sellListTableView_B->currentIndex().row();
    if(row < 0)
    {
        return;
    }
    billModel_B->removeRow(row);
}

void MainForm::on_sellManComboBox_B_currentIndexChanged(const QString &arg1)    //历史账单中筛选
{
    updateBillPageSlot();
}

void MainForm::on_goodsTypeComboBox_M_currentIndexChanged(const QString &arg1)  //当商品类型改变时
{
    QString curTypeName = ui->goodsTypeComboBox_M->currentText();       //获取当前商品类别名
    QSqlQuery query;
    if(curTypeName == tr("查看所有商品"))
    {
        goodsNameModel_M->setFilter(QString(""));//设置过滤器为空，就会显示所有的数据
    }
    else
    {
        query.exec(QString("select id from goodsTypeTb where typename = '%1'").arg(curTypeName));
        if(query.next())                //如果有数据
        {
            this->goodsNameModel_M->setFilter(QString("typeId=%1").arg(query.value(0).toInt()));//条件
            goodsNameModel_M->select();           //执行该语句
        }
    }

    ui->goodsNameTableView_M->selectRow(0);     //当类型改变时，选择第一项
    on_goodsNameTableView_M_clicked(goodsNameModel_M->index(0,0));  //将第一行的数据显示
}

void MainForm::on_goodsNameDelBtn_M_clicked()           //商品界面删除
{
    int row  = ui->goodsNameTableView_M->currentIndex().row();     //获取视图选中的行
    QSqlRecord record = goodsNameModel_M->record(row);
    QString goodsName = record.value(GoodsName_Name).toString();
    int result = QMessageBox::warning(this,tr("警告"),tr("确定删除\"%1\"商品吗?").arg(goodsName)
                                       ,QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(result != QMessageBox::Yes)      //警告是否删除商品
        return;

    QSqlDatabase::database().transaction();     //开启数据库事务操作
    bool ok = goodsNameModel_M->removeRow(row,QModelIndex());     //删除第row行
    if(ok)
    {
        QSqlDatabase::database().commit();          //接受
        QMessageBox::information(this,tr("提示"),tr("成功删除\"%1\"商品").arg(goodsName));
    }
    else
    {
        QSqlDatabase::database().rollback();        //回滚
        QMessageBox::warning(this,tr("错误"),tr("删除\"%1\"商品失败!\n原因：%2")
                             .arg(goodsName).arg(QSqlDatabase::database().lastError().text()));
        return;
    }

    if(row != 0)        //如果删除的不是第0行，就选中前一行
    {
        ui->goodsNameTableView_M->selectRow(row - 1);
    }
    else
    {
        ui->goodsNameTableView_M->selectRow(0);        //否则选中第0行
    }
    ui->goodsNameTableView_M->setFocus();      //设置view焦点
}

void MainForm::on_goodsNameTableView_M_clicked(const QModelIndex &index)    //阅览商品时单击商品
{
    QSqlRecord record = goodsNameModel_M->record(index.row());
    ui->goodsIdLineEdit_M->setText(record.value("num").toString());
    ui->goodsNameLineEdit_M->setText(record.value("name").toString());
    //显示商品的类别信息
    QString name = record.value(GoodsName_TypeId).toString();   //首先取出类别名
    QSqlQuery query;
    query.exec(tr("select *from goodsTypeTb where typename = '%1'").arg(name));//根据类型名取出id号
    query.next();
    ui->addGoodsTypeCombox_M->setCurrentIndex(query.value(0).toInt() - 1);  //根据id号选择当前的index
    ui->stockSpinBox_M->setValue(record.value("stock").toInt());
    ui->bidSpinBox_M->setValue(record.value("bid").toInt());
    ui->retailSpinBox_M->setValue(record.value("retail").toInt());
    ui->formatLineEdit_M->setText(record.value("format").toString());
    ui->unitLineEdit_M->setText(record.value("unit").toString());
    ui->noteTextEdit_M->setText(record.value("note").toString());
}

void MainForm::setWidgetStatus_M(bool isEnable)     //设置商品列表界面控件的状态
{
    ui->goodsIdLineEdit_M->setEnabled(isEnable);
    ui->goodsNameLineEdit_M->setEnabled(isEnable);
    ui->addGoodsTypeCombox_M->setEnabled(isEnable);
    ui->stockSpinBox_M->setEnabled(isEnable);
    ui->bidSpinBox_M->setEnabled(isEnable);
    ui->retailSpinBox_M->setEnabled(isEnable);
    ui->formatLineEdit_M->setEnabled(isEnable);
    ui->unitLineEdit_M->setEnabled(isEnable);
    ui->noteTextEdit_M->setEnabled(isEnable);
    ui->submitBtn_M->setEnabled(isEnable);
}

void MainForm::on_addGoodsBtn_M_clicked()           //添加商品
{
    setWidgetStatus_M(true);        //设置所有控件为可用
    ui->goodsIdLineEdit_M->clear();
    ui->goodsNameLineEdit_M->clear();
    ui->stockSpinBox_M->setValue(0);
    ui->bidSpinBox_M->setValue(0.0);
    ui->retailSpinBox_M->setValue(0.0);
    ui->formatLineEdit_M->clear();
    ui->unitLineEdit_M->clear();
    ui->noteTextEdit_M->clear();

    ui->goodsIdLineEdit_M->setFocus();
    submitState = Submit_Insert;        //更改状态
}

void MainForm::on_submitBtn_M_clicked()     //提交商品提交按钮
{
    if(ui->goodsIdLineEdit_M->text().isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("商品条码不得为空"));
        ui->goodsIdLineEdit_M->setFocus();
        return;
    }
    if(ui->goodsNameLineEdit_M->text().isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("商品名不得为空"));
        ui->goodsNameLineEdit_M->setFocus();
        return;
    }
    //添加商品或者修改商品信息，二者要分开处理，添加是insert
    if(submitState == Submit_Insert)        //如果是添加商品
    {
        //查看是否已经有了该商品
        QSqlQuery query;
        query.exec(tr("select *from goodsNameTb where num = '%1'").arg(ui->goodsIdLineEdit_M->text()));
        if(query.next())
        {
            QMessageBox::warning(this,tr("警告"),tr("该商品已经存在，请勿重复添加！"));
            return;
        }
        //如果该商品不存在，就添加该商品
        query.prepare(tr("insert into goodsNameTb values(:num,:type,:name,:stock,:bid,:retail,:format,:unit,:note)"));
        query.bindValue(":num",ui->goodsIdLineEdit_M->text());  //条码
        QSqlQuery typeQuery;
        typeQuery.exec(tr("select *from goodsTypeTb where typename = '%1'").arg(ui->addGoodsTypeCombox_M->currentText()));
        typeQuery.next();
        query.bindValue(":type",typeQuery.value(0).toInt());         //类别
        query.bindValue(":name",ui->goodsNameLineEdit_M->text());
        query.bindValue(":stock",ui->stockSpinBox_M->value());  //存货
        query.bindValue(":bid",ui->bidSpinBox_M->value());      //进价
        query.bindValue(":retail",ui->retailSpinBox_M->value());        //零售价
        query.bindValue(":format",ui->formatLineEdit_M->text());        //规格
        query.bindValue(":unit",ui->unitLineEdit_M->text());            //单位
        QString noteText = ui->noteTextEdit_M->toPlainText();
        query.bindValue(":note",noteText.isEmpty() ? "none":noteText);      //备注
        if(query.exec())
        {
            on_goodsTypeComboBox_M_currentIndexChanged("");
            ui->goodsNameTableView_M->selectRow(goodsNameModel_M->rowCount() - 1);
            on_addGoodsBtn_M_clicked();     //调用私有函数，重置所有控件值
        }
        else
        {
            QMessageBox::warning(this,tr("警告"),tr("商品添加失败!\n原因：%1").arg(query.lastError().text()));
        }
    }
    else if(submitState == Submit_Update)       //如果是更新商品的信息,提示：不能修改主键值
    {
        QSqlQuery query;
        query.exec(tr("update goodsNameTb set typeId = :type,name = :name,stock = :stock,bid = :bid,"
                              "retail = :retail,format= :format,unit= :unit,note=:note where num = '%1'")
                           .arg(ui->goodsIdLineEdit_M->text()));
        QSqlQuery typeQuery;
        typeQuery.exec(tr("select *from goodsTypeTb where typename = '%1'").arg(ui->addGoodsTypeCombox_M->currentText()));
        typeQuery.next();
        query.bindValue(":typeId",typeQuery.value(0).toInt());         //类别
        query.bindValue(":name",ui->goodsNameLineEdit_M->text());
        query.bindValue(":stock",ui->stockSpinBox_M->value());  //存货
        query.bindValue(":bid",ui->bidSpinBox_M->value());      //进价
        query.bindValue(":retail",ui->retailSpinBox_M->value());        //零售价
        query.bindValue(":format",ui->formatLineEdit_M->text());        //规格
        query.bindValue(":unit",ui->unitLineEdit_M->text());            //单位
        QString noteText = ui->noteTextEdit_M->toPlainText();
        query.bindValue(":note",noteText.isEmpty() ? "none":noteText);      //备注
        QSqlDatabase::database().transaction();     //开启事务操作
        bool ok = query.exec();
        if(ok)
        {
            QSqlDatabase::database().commit();
            QMessageBox::information(this,tr("提示"),tr("商品信息更改成功!"));
            on_searchLineEdit_M_textChanged("");
        }
        else
        {
            QSqlDatabase::database().rollback();
            QMessageBox::warning(this,tr("警告"),tr("商品信息修改失败!原因：%1").arg(query.lastError().text()));
        }
    }
}

void MainForm::on_reviseBtn_M_clicked()         //商品界面修改按钮
{
    if(! ui->goodsNameTableView_M->currentIndex().isValid())
    {
        QMessageBox::warning(this,tr("警告"),tr("请选择需要更改的商品"));
    }
    submitState = Submit_Update;        //更改状态，从而使提交按钮根据情况添加或更改
    this->setWidgetStatus_M(true);      //设置所有控件为可用
    on_goodsNameTableView_M_clicked(ui->goodsNameTableView_M->currentIndex());  //将当前选中的显示
}

void MainForm::on_searchLineEdit_M_textChanged(const QString &arg1)     //商品的模糊查询
{
    QString type = ui->goodsTypeComboBox_M->currentText();
    QSqlQuery query;
    query.exec(tr("select id from goodsTypeTb where typename = '%1'").arg(type));
    query.next();
    if(type == tr("查看所有商品"))            //如果是“查看所有商品”,就不需要商品类别这个条件
    {
        goodsNameModel_M->setFilter(tr("name like '%1%' or num like '%2%'")
                                    .arg(arg1).arg(arg1));
    }
    else
    {
        goodsNameModel_M->setFilter(tr("name like '%1%' or num like '%2%' and typeId = %3")
                                    .arg(arg1).arg(arg1).arg(query.value(0).toInt()));
    }
}

void MainForm::purchaseDetailSlot()
{
    ui->stackedWidget->setCurrentIndex(4);
    if(! purchaseModel_P)
    {
        purchaseModel_P = new QSqlRelationalTableModel(this);       //进货单模型
        purchaseModel_P->setTable("purchaseTb");
        purchaseModel_P->setRelation(Purchase_SupplierId,QSqlRelation("supplierTb","id","name"));
        purchaseModel_P->select();

        purchaseModel_P->setHeaderData(Purchase_Id,Qt::Horizontal,tr("进货单号"));
        purchaseModel_P->setHeaderData(Purchase_DateTime,Qt::Horizontal,tr("进货时间"));
        purchaseModel_P->setHeaderData(Purchase_SupplierId,Qt::Horizontal,tr("供货商"));
        purchaseModel_P->setHeaderData(Purchase_TotalPrice,Qt::Horizontal,tr("总额"));
        purchaseModel_P->setHeaderData(Purchase_Handelr,Qt::Horizontal,tr("处理者"));
        purchaseModel_P->setHeaderData(Purchase_Note,Qt::Horizontal,tr("备注"));
        //进货清单详细
        purchaseDetailModel_P  = new QSqlRelationalTableModel(this);
        purchaseDetailModel_P->setTable("purchaseDetailTb");
        purchaseDetailModel_P->setRelation(PurDetail_GoodsId,QSqlRelation("goodsNameTb","num","name"));
        purchaseDetailModel_P->select();
        purchaseDetailModel_P->setHeaderData(PurDetail_PurchaseId,Qt::Horizontal,tr("进货单号"));
        purchaseDetailModel_P->setHeaderData(PurDetail_GoodsId,Qt::Horizontal,tr("商品名称"));
        purchaseDetailModel_P->setHeaderData(PurDetail_Count,Qt::Horizontal,tr("商品数量"));

        ui->purchaseTableView_P->setModel(purchaseModel_P);
        ui->purchaseTableView_P->resizeColumnsToContents();
        ui->purchaseTableView_P->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->purchaseTableView_P->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->purchaseTableView_P->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->purchaseTableView_P->horizontalHeader()->setStretchLastSection(true);
        copyAction_P = new QAction(tr("复制单号"),this);            //视图的右键菜单，可以复制单号
        connect(copyAction_P,SIGNAL(triggered()),this,SLOT(copyPurchaseId()));
        ui->purchaseTableView_P->addAction(copyAction_P);
        ui->purchaseTableView_P->setContextMenuPolicy(Qt::ActionsContextMenu);

        ui->purDetailTableView_P->setModel(purchaseDetailModel_P);
        ui->purDetailTableView_P->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->purDetailTableView_P->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->purDetailTableView_P->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->purDetailTableView_P->horizontalHeader()->setStretchLastSection(true);
        ui->purDetailTableView_P->setColumnHidden(PurDetail_Id,true);
        ui->purDetailTableView_P->setColumnWidth(PurDetail_PurchaseId,200);
        ui->purDetailTableView_P->setColumnWidth(PurDetail_GoodsId,200);
        //设置查询时间
        QSqlQuery query;
        query.exec(tr("select dateTime from purchaseTb"));
        if(query.last())
        {
            ui->startDateEdit_P->setDate(query.value(0).toDate());
        }
        else
        {
            ui->startDateEdit_P->setDate(QDate::currentDate());
        }
        ui->endDateEdit_P->setDate(QDate::currentDate());
    }
}


void MainForm::copyPurchaseId()         //右键菜单复制单号
{
    int row = ui->purchaseTableView_P->currentIndex().row();
    QString str = purchaseModel_P->index(row,0).data().toString();
    QApplication::clipboard()->setText(str);
}

void MainForm::updatePurchaseView()     //更新进货界面的信息，如新添加了货单以及重新计算总价
{

    QString id = ui->purchaseIdLineEdit_P->text();
    QString startDate = ui->startDateEdit_P->date().toString("yyyy-MM-dd");
    QString endDate = ui->endDateEdit_P->date().toString("yyyy-MM-dd");
    purchaseModel_P->setFilter(tr("num like '%1%' and (dateTime >= '%2' and dateTime <= '%3')")
                               .arg(id).arg(startDate).arg(endDate));

    purchaseModel_P->select();
    if(purchaseModel_P->rowCount() > 0)         //如果筛选后还有数据，就选中第一项
    {
        ui->purchaseTableView_P->selectRow(0);
        on_purchaseTableView_P_clicked(purchaseModel_P->index(0,0));//相当于单击第一项
    }
    else            //如果没有数据，就清空清单的数据
    {
        purchaseDetailModel_P->setFilter(tr("id = -1"));
        purchaseDetailModel_P->select();
    }
    //计算进货单的总价
    double totalPrice = 0.0;
    for(int row = 0;row < purchaseModel_P->rowCount();row++)        //遍历整个模型，计算价格
    {
        totalPrice += purchaseModel_P->index(row,Purchase_TotalPrice).data().toDouble();
    }
    ui->purchaseLabel_P->setText(tr("总价：%1").arg(QString::number(totalPrice)));
}

void MainForm::purchase()   //主界面的进货按钮弹出进货对话框
{
    AddPurchaseDlg dlg(userName);
    dlg.exec();
 /*   if(dlg.exec() == QDialog::Accepted)     //如果成功的进货
    {
        updatePurchaseView();       //更新界面信息
    } */
}

void MainForm::addGoodsType()       //添加商品类别
{
    AddGoodTypeDlg dlg;
    if(dlg.exec() == QDialog::Accepted)
    {

    }
}

void MainForm::on_addPurchase_P_clicked()       //进货按钮
{
    AddPurchaseDlg dlg(userName);
    if(dlg.exec() == QDialog::Accepted)     //如果成功的进货
    {
        updatePurchaseView();       //更新界面信息
    }
}

void MainForm::on_purchaseIdLineEdit_P_textChanged(const QString &arg1)
{
    updatePurchaseView();       //当单号发生改变就更新视图进行筛选
}

void MainForm::on_startDateEdit_P_dateChanged(const QDate &date)
{
    updatePurchaseView();
}

void MainForm::on_endDateEdit_P_dateChanged(const QDate &date)  //当查询时间发生改变
{
    updatePurchaseView();
}

void MainForm::on_purchaseTableView_P_clicked(const QModelIndex &index)     //单击相应的进货单
{
    int row = index.row();
    QModelIndex temp =  purchaseModel_P->index(row,0);
    if(temp.isValid())
    {
        purchaseDetailModel_P->setFilter(tr("purchaseId = '%1'").arg(temp.data(Qt::DisplayRole).toString()));
        purchaseDetailModel_P->select();
    }
    //计算每一单的价格并显示
    double price = purchaseModel_P->index(row,Purchase_TotalPrice).data().toDouble();
    ui->purDetailLabel_P->setText(tr("总价：%1").arg(QString::number(price)));
}



void MainForm::showSupplierSlot()  //添加供应商
{
    SupplierDlg dlg;
    ui->stackedWidget->setCurrentIndex(6);  //管理供应商界面时，将后面的主页面跳转到首页
    dlg.exec();
}

void MainForm::clickInfor(QString text)
{
    if(text == tr("历史账单"))
    {
        showHistoryBillSlot();
    }
    else if(text == tr("收银台"))
    {
        showSellPageSlot();
    }
    else if(text == tr("出售统计"))
    {
        showSallStatisticsSlot();
    }
    else if(text == tr("商品信息"))
    {
        showGoodsInforSlot();
    }
    else if(text == tr("供应商管理"))
    {
        showSupplierSlot();
    }
    else if(text == tr("用户管理"))
    {
        this->showUserManagementSlot();
    }
    else if(text == tr("进货明细"))
    {
        this->purchaseDetailSlot();
    }
    else if(text == tr("进货处理"))
    {
        this->purchase();
    }
    else if(text == tr("商品类别"))
    {
        addGoodsType();
    }
}

void MainForm::on_AddBtn_U_clicked()    //用户页面的添加按钮
{
    AddUserDlg dlg;
    if(dlg.exec() == QDialog::Accepted)
    {
        this->showUserManagementSlot(); //刷新页面
    }
}

void MainForm::on_DeleteBtn_U_clicked() //用户页面的删除按钮
{
    if(ui->UserTableView_U->currentIndex().row() == -1)
    {
        //-1说明没有选择
        QMessageBox::warning(this,tr("警告"),tr("请选择需要删除的用户！"));
    }
}

void MainForm::on_ModifyBtn_U_clicked() //用户页面的修改按钮
{
    int row =  ui->UserTableView_U->currentIndex().row();
    if(row < 0)
    {
        QMessageBox::warning(this,tr("警告"),tr("请选择要修改的用户！"),QMessageBox::Ok);
        return;
    }
    QString username = UserManagementModel_U->index(row,0).data().toString();
    ModifyUserDlg dlg(username);
    if(dlg.exec() == QDialog::Accepted)
    {
        this->showUserManagementSlot();
    }
}

void MainForm::on_okBtn_S_clicked() //收银台界面的商品条码号的确定按钮
{
    QString goodsNum = ui->barcodeLineEdit_S->text();//获取输入的条码
    if(goodsNum.isEmpty())  //如果条码为空，直接返回
        return;
    QSqlQuery query;
    query.exec(tr("SELECT *FROM goodsNameTb WHERE num = '%1'").arg(goodsNum));
    if(query.next())
    {
        destoryCompleter();
        ui->barcodeLineEdit_S->setFocus();
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),tr("输入的条码号有误，请重新输入！"));
        ui->barcodeLineEdit_S->selectAll();
        ui->barcodeLineEdit_S->setFocus();
    }
}
