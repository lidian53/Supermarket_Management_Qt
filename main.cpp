#include "logindlg.h"
#include <QApplication>
#include <QTextCodec>
#include "mainform.h"
#include "connection.h"
#include <QTextCodec>
#include "collectdlg.h"
#include "choicegoodsdlg.h"
#include "addpurchasedlg.h"
#include "supplierdlg.h"
#include "addsupplierdlg.h"

#include "adduserdlg.h" //测试添加用户
#include "modifyuserdlg.h"
#include "addgoodtypedlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));   //设置编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//设置编码
    bool isExist = QFileInfo("MarketDb.db").exists();
    if(! createConnection())        //调用静态函数建立数据库
        return 1;
    if(! isExist)     //查看数据库文件是否已经存在，如果不存在就添加数据
    {
        addData();      //调用静态函数添加数据
    }
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
   // collectDlg dlg(12);
    //dlg.show();
  /*  ChoiceGoodsDlg dlg;
    dlg.show();
    return a.exec(); */

/*    MainForm form;
     form.show();
     return a.exec(); */
 /*   SupplierDlg dlg;
    dlg.show();
    return a.exec(); */
    /*   AddSupplierDlg dlg;
    dlg.show();
    return a.exec(); */

  /*  AddPurchaseDlg dlg("admin");
    dlg.show();
    return a.exec(); */

/*    AddUserDlg dlg;
    dlg.show();
    return a.exec(); */

/*    ModifyUserDlg dlg("admin");
    dlg.show();
    return a.exec(); */

/*    AddGoodTypeDlg dlg;
    dlg.exec();
    return a.exec(); */

    LoginDlg w;
    if(w.exec() == QDialog::Accepted)
    {
        MainForm form(w.getUserName());
       // form.setUserName(w.getUserName());
        form.show();
        return a.exec();
    }
    else
    {
        return 1;
    }
}
