#include "modifyuserdlg.h"
#include "ui_modifyuserdlg.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

ModifyUserDlg::ModifyUserDlg(QString username, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyUserDlg)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->username = username;

    initView();     //初始化窗口各信息
    //设置身份证号码文本框的正则表达式，只能输入18位的数字
    QRegExp regExp("[0-9]{18}");

    ui->idCardLineEdit->setValidator(new QRegExpValidator(regExp, this));
}

ModifyUserDlg::~ModifyUserDlg()
{
    delete ui;
}

void ModifyUserDlg::on_cancelBtn_clicked()
{
    this->reject();
}

void ModifyUserDlg::on_okBtn_clicked()
{

    QString password = ui->pswLineEdit->text();
    QString jobNum = ui->jobNumLineEdit->text();
    QString IDCardNum = ui->idCardLineEdit->text();
    QString sex = ui->sexComboBox->currentText();
    if(password.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("密码不得为空!"));
        ui->pswLineEdit->setFocus();
        return;
    }
    else if(jobNum.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("工作号不得为空!"));
        ui->jobNumLineEdit->setFocus();
        return;
    }
    else if(IDCardNum.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("身份证号不得为空!"));
        ui->idCardLineEdit->setFocus();
        return;
    }
    else if(IDCardNum.length() != 18)
    {
        QMessageBox::warning(this,tr("警告"),tr("身份证号填写不正确!"));
        ui->idCardLineEdit->setFocus();
        return;
    }
    //修改用户信息
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare(tr("UPDATE userTb SET password=:password ,jobNum=:jobNum,sex=:sex,"
                     "IDcardNum=:idcard WHERE username=:username"));
    query.bindValue(":password",password);
    query.bindValue(":jobNum",jobNum);
    query.bindValue(":sex",sex);
    query.bindValue(":IDcardNum",IDCardNum);
    query.bindValue(":username",username);
    if(query.exec())
    {
        QMessageBox::information(this,tr("提示"),tr("修改成功!"),QMessageBox::Ok);
        QSqlDatabase::database().commit();  //提交
        this->accept();
    }
    else
    {
        QMessageBox::critical(this,tr("严重错误"),tr("抱歉！信息修改失败！\n错误原因：%1")
                              .arg(query.lastError().text()));
        QSqlDatabase::database().rollback();    //回滚
    }
}

void ModifyUserDlg::paintEvent(QPaintEvent *event)
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

void ModifyUserDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void ModifyUserDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void ModifyUserDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void ModifyUserDlg::initView()  //初始化窗口
{
    ui->usernameLineEdit->setText(username);
    ui->usernameLineEdit->setEnabled(false);    //设置用户名不得更改

    QSqlQuery query;
    QString password;
    QString jobNum;
    QString IDCardNum;
    QString sex;
    query.exec(tr("SELECT password,jobNum,sex,IDCardNum FROM userTb WHERE username = '%1'")
               .arg(username));
    if(query.next()) {
        password = query.value(0).toString();
        jobNum = query.value(1).toString();
        sex = query.value(2).toString();
        IDCardNum = query.value(3).toString();
    }
    ui->pswLineEdit->setText(password);
    ui->jobNumLineEdit->setText(jobNum);
    ui->idCardLineEdit->setText(IDCardNum);
    int index = sex == tr("男") ? 0 : 1;
    ui->sexComboBox->setCurrentIndex(index);

    ui->pswLineEdit->setFocus();

}
