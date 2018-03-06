#include "logindlg.h"
#include "ui_logindlg.h"
#include <QMessageBox>
#include <QMouseEvent>
#include "connection.h"
#include "showinforlabel.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <qmath.h>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);  //设置窗体为无边框
    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->userNameLineEdit->setFocus();
    ui->loginBtn->setDefault(true);             //设置登录按钮为默认按钮(Enter可触发)
    errorLabel = new ShowInforLabel(this);          //显示密码不正确等错误信息的label
    errorLabel->setVisible(false);          //先不显示
    readSetting();
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

QString LoginDlg::getUserName() const
{
    return ui->userNameLineEdit->text();
}

void LoginDlg::on_loginBtn_clicked()        //登录按钮单击
{
    if(ui->userNameLineEdit->text().isEmpty())      //判断用户名是否为空
    {
        this->setErrorLabelText(tr("请输入用户名"))   ;
        ui->userNameLineEdit->setFocus();
        return;
    }
    QString name = ui->userNameLineEdit->text().trimmed();  //取出用户名
    QString password = ui->passwordLineEdit->text();        //取出密码
    QSqlQuery query(tr("select username from userTb"));//遍历表中所有的用户名
    bool hasUser = false;
    while(query.next())
    {
        if(query.value(0).toString() == name)
        {
            hasUser = true;
            break;
        }
    }
    if(hasUser) //如果有这个用户
    {
        query.exec(QString("select password from userTb where username = '%1'").arg(name));
        query.next();
        if(query.value(0).toString() == password)           //如果用户名以及密码都正确，就接受
        {
            accept();
        }
        else
        {
            this->setErrorLabelText(tr("密码不正确，请重新输入"),QPoint(80,125));  //调用自定义函数，进行显示错误信息
            ui->passwordLineEdit->selectAll();
            ui->passwordLineEdit->setFocus();
        }
    }
    else
    {
        this->setErrorLabelText(tr("对不起，没有该用户，请重新输入"));
        ui->userNameLineEdit->selectAll();
        ui->userNameLineEdit->setFocus();
    }
}

void LoginDlg::setLabelOpacity()                    //执行自定义属性“opacity”动画
{
    QPropertyAnimation *animation = new QPropertyAnimation(errorLabel,"opacity");

    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setDuration(1500);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation,SIGNAL(finished()),errorLabel,SLOT(setWindowVisable()));//当结束时，设置label为不可见
}


void LoginDlg::mousePressEvent(QMouseEvent *event)      //拉动窗体
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 70)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void LoginDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void LoginDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void LoginDlg::paintEvent(QPaintEvent *event)
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

void LoginDlg::setErrorLabelText(const QString &text, const QPoint &point)
{
    if(errorLabel->isVisible())     //避免出现动画还没stop，就按下第二次确定的重复动画
        return;
    errorLabel->setText(text);          //设置errorlabel的显示文本
    errorLabel->setGeometry(point.x(),point.y(),            //设置errorLabel的位置，以及根据文本确定宽度
                            fontMetrics().width(text) + 30,40);
    errorLabel->setVisible(true);          //设置label为可见
    QTimer::singleShot(500,this,SLOT(setLabelOpacity()));      //显示1秒后，执行隐藏动画
}

void LoginDlg::readSetting()            //读取配置
{
    QSettings settings("Software Inc.","LoginDlg");
    QString userName = settings.value("UserName").toString();
    bool checked = settings.value("Checked").toBool();
    ui->userNameLineEdit->setText(userName);
    ui->checkBox->setChecked(checked);
    if(! userName.isEmpty())
    {
        ui->passwordLineEdit->setFocus();
    }
}

void LoginDlg::writeSetting()           //写入配置
{
    QSettings settings("Software Inc.", "LoginDlg");
    QString name;
    bool checked;
    if(ui->checkBox->isChecked())
    {
        name = ui->userNameLineEdit->text().trimmed();
        checked = true;
    }
    else
    {
        checked = false;
        name = QString("");
    }

    settings.setValue("UserName",name);
    settings.setValue("Checked",ui->checkBox->isChecked());
}

void LoginDlg::accept()
{
    writeSetting();
    QDialog::accept();
}

void LoginDlg::on_checkBox_clicked(bool checked)
{

}
