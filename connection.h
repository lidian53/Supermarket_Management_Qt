    #ifndef CONNECTION_H
#define CONNECTION_H
#include <QtSql>
#include <QMessageBox>
#include <QProgressDialog>
/*
建立数据库以及表
*/
static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("MarketDb.db");
    db.setUserName(QString("hxb"));
    db.setPassword(QString("123456"));
    if(! db.open())
    {
        QMessageBox::critical(0,QString("严重错误")
                              ,QString("无法创建数据库\n原因：%1").arg(db.lastError().text()),
                              QMessageBox::Ok);
        return false;
    }
    return true;
}

static void addData()
{
    QProgressDialog dialog;         //创建一个进度条，显示添加数据的进度
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowTitle(QObject::tr("Manager"));
    dialog.setLabelText(QObject::tr("Creating database..."));
    dialog.setMinimum(0);
    dialog.setMaximum(10);
    dialog.show();
    qApp->processEvents();
    dialog.setValue(0);
    QSqlQuery query;
    query.exec(QString("create table userTb (username varchar(15) primary key,"
                       "password varchar(20),"
                       "jobNum varchar(15) not null,"
                       "sex varchar(2) not null,"
                       "IDcardNum varchar(18) not null,lastLogTime datetime not null)"));
    query.exec(QString("insert into userTb values('admin','123456','20150905003','女','330523199509265212','2016-09-02 12:12:32')"));
    query.exec(QString("insert into userTb values('hxb','123456','20160805002','男','330523199306202512','2016-09-02 12:12:32')"));
    qApp->processEvents();
    dialog.setValue(1);
    //商品类别表
    query.exec(QString("create table goodsTypeTb (id int primary key,typename varchar(20) not null)"));
    query.exec(QString("insert into goodsTypeTb values(00,'查看所有商品')"));
    query.exec(QString("insert into goodsTypeTb values(01,'饮料类')"));
    query.exec(QString("insert into goodsTypeTb values(02,'零食类')"));
    query.exec(QString("insert into goodsTypeTb values(03,'学习用品类')"));
    qApp->processEvents();
    dialog.setValue(2);

    //商品表
    query.exec(QString("create table goodsNameTb (num varchar(15) primary key,typeId int"
                       ",name varchar(30) not null,stock int not null,bid double not null"
                       ",retail double not null,"
                       "format varchar(10) not null,unit varchar(1) not null,note varchar(50),"
                       "foreign key(typeId) references goodsTypeTb)"));
    query.exec(QString("insert into goodsNameTb values('912543524',01,'百事可乐',98,3.7,4.2,'450ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('934210249',01,'营养快线',42,3,4.2,'450ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('915346324',02,'乐事薯片',54,2,3,'200g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('950024153',02,'德芙巧克力',13,3,4,'100g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('965346813',01,'养乐多',64,3,5,'250ML','瓶','none')"));
    qApp->processEvents();
    dialog.setValue(3);
    query.exec(QString("insert into goodsNameTb values('965437145',02,'彩虹糖',7,3,5,'80g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('965789786',01,'七喜',46,3,4,'450ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('976407358',02,'闲趣饼干',25,3,4,'400g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('930700077',01,'脉动',534,3,4,'450ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('912311114',02,'卫龙辣条（大）',14,3,4,'300g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('963876357',01,'冰红茶',67,3,4.8,'400ML','瓶','none')"));
    qApp->processEvents();
    dialog.setValue(4);
    query.exec(QString("insert into goodsNameTb values('963786371',02,'凤爪',85,3,4.6,'100g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('935672615',01,'+C',98,3,4.1,'400ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('900008874',01,'恒大矿泉水',46,1,2.2,'400ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('943534643',02,'鲜虾饼',34,3,4,'100g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('914546345',02,'火腿肠',14,3,4,'70g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('946346346',01,'橙汁',65,3,4,'800ML','瓶','none')"));
    qApp->processEvents();
    dialog.setValue(5);
    query.exec(QString("insert into goodsNameTb values('901410011',01,'可口可乐',85,3,4,'800ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('951356716',01,'柠檬水',85,3,4,'450ML','瓶','none')"));
    query.exec(QString("insert into goodsNameTb values('916572367',02,'肉松饼',96,3,4,'50g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('935720175',02,'鸡爪',998,3,4,'70g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('963767371',02,'康师傅统一方便面',24,3,4,'300g','桶','none')"));
    query.exec(QString("insert into goodsNameTb values('923658362',02,'绿箭口香糖',93,4,5,'30g','盒','none')"));
    qApp->processEvents();
    dialog.setValue(6);
    query.exec(QString("insert into goodsNameTb values('916358262',02,'炫迈口香糖',35,3,4,'30g','盒','none')"));
    query.exec(QString("insert into goodsNameTb values('973876363',02,'益达口香糖',3,3,4,'30g','盒','none')"));
    query.exec(QString("insert into goodsNameTb values('907948784',02,'菜园小饼',63,3,4,'100g','袋','none')"));
    query.exec(QString("insert into goodsNameTb values('900637496',02,'巧乐兹',25,3,4,'120g','袋','none')"));
    qApp->processEvents();
    dialog.setValue(7);

    //出售表  订单号，售货员，出售时间，实际价格，收银
    query.exec(QString("create table sellTb(sellId varchar(15) not null,"
                                    "sellMan varchar(15) not null references userTb(username),"
                                    "sellDateTime datetime not null,"
                                    "factPrice double not null,"
                                    "collectPrice double not null,sellCount int not null)"));
    query.exec(QString("insert into sellTb values('20160902_001','admin','2016-09-02 12:12:32',100.00,30.00,5)"));
    qApp->processEvents();
    dialog.setValue(8);
    /*
    出售表：每一件商品单独成一条数据，每一次购买商品有一个唯一的sellId，而每一条数据有一个唯一的serialNum标记

    */
    query.exec(QString("create table sellListTb(serialNum int primary key not null,"
                       "sellId varchar(15) not null references sellTb(sellId),"
                       "todayCount int not null,"
                       "goodsId varchar(15) references goodsNameTb(num),"
                       "count int not null,"
                       "dateTime date not null)"));
    qApp->processEvents();
    dialog.setValue(9);
    //供应商表
    query.exec(QString("create table supplierTb(id int primary key not null,name varchar(15) not null,"
                  "contact varchar(10) not null,telephone varchar(11) not null,address varchar(20),"
                  "note varchar(30))"));
    query.exec(QString("insert into supplierTb values(001,'杭州华联批发商城','张胖胖','13857127973','杭州市','none')"));
    //进货表
    query.exec(QString("create table purchaseTb(num varchar(18) not null primary key,dateTime date not null,"
                  "supplierId int not null references supplierTb(id),totalPrice double not null,"
                  "handler varchar(15) not null references userTb(username),note varchar(30))"));
    query.exec(QString("insert into purchaseTb values('123242141','2016-09-07','1',123.51,'admin','note')"));
    //进货明细表
    query.exec(QString("create table purchaseDetailTb(id int primary key not null,"
                       "purchaseId varchar(18) not null references purchaseTb(id),"
                       "goodsId varchar(15) references goodsNameTb(num),"
                       "count int not null)"));
    qApp->processEvents();
    dialog.setValue(10);
}

#endif // CONNECTION_H
