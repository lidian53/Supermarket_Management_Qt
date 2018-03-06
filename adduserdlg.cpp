#include "adduserdlg.h"
#include "ui_adduserdlg.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include <QSqlQuery>
#include <QMessageBox>
#include "showinforlabel.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QRegExpValidator>
#include <QDateTime>
#include <QSqlError>

AddUserDlg::AddUserDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUserDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->pswLineEdit->setEchoMode(QLineEdit::Password);

    this->initEvent();
    ui->usernameLineEdit->setFocus();

    errorLabel = new ShowInforLabel(this);  //错误信息
    errorLabel->setVisible(false);

    //设置身份证号码文本框的正则表达式，只能输入18位的数字
    QRegExp regExp("[0-9]{18}");

    ui->idCardLineEdit->setValidator(new QRegExpValidator(regExp, this));
}

AddUserDlg::~AddUserDlg()
{
    delete ui;
}

//事件过滤器，如果是回车，就跳转到下一个控件，方便输入
bool AddUserDlg::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->usernameLineEdit || target == ui->pswLineEdit || target == ui->jobNumLineEdit
            || target == ui->sexComboBox || target == ui->idCardLineEdit)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Return || keyEvent->key() ==  Qt::Key_Enter)
            {
                this->focusNextChild();
                return true;
            }
        }
    }
    return QDialog::eventFilter(target,event);
}

void AddUserDlg::setLabelOpacity()  //错误信息的消失渐变动画
{
    QPropertyAnimation *animation = new QPropertyAnimation(errorLabel,"opacity");

    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setDuration(1500);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation,SIGNAL(finished()),errorLabel,SLOT(setWindowVisable()));//当结束时，设置label为不可见
}

void AddUserDlg::on_okBtn_clicked()     //确定按钮，提交用户
{
    //判断所有信息是否都填写，没有就提示错误
    if(ui->usernameLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入用户名"),QPoint(ui->usernameLineEdit->pos() + QPoint(-12,10)));
        ui->usernameLineEdit->setFocus();
        return;
    }
    else if(ui->pswLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入密码"),QPoint(ui->pswLineEdit->pos() + QPoint(-12,10)));
        ui->pswLineEdit->setFocus();
        return;
    }
    else if(ui->jobNumLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入工作号"),QPoint(ui->jobNumLineEdit->pos() + QPoint(-12,10)));
        ui->jobNumLineEdit->setFocus();
        return;
    }
    else if(ui->idCardLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入身份证号码"),QPoint(ui->idCardLineEdit->pos() + QPoint(-12,10)));
        ui->idCardLineEdit->setFocus();
        return;
    }
    else if(ui->idCardLineEdit->text().length() != 18) //如果不是18位就重新输入
    {
        this->setErrorLabelText(tr("身份证号输入不正确"),QPoint(ui->idCardLineEdit->pos() + QPoint(-12,10)));
        ui->idCardLineEdit->setFocus();
        return;
    }


    QSqlQuery query;
    QString username = ui->usernameLineEdit->text();
    //根据输入的用户名判断数据表中是否有这个用户
    query.exec(tr("SELECT * FROM userTb WHERE username='%1'").arg(username));
    if(query.next()) {
        //如果已经有了这个用户
        QMessageBox::warning(this,tr("警告"),tr("这个用户已经存在，请输入一个新的用户！"));
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->selectAll();
        return ;    //选中用户名文本框，并直接返回
    }
    QString curDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    //所有信息都已经符合标准就往数据表存储
    query.prepare("INSERT INTO userTb(username,password,jobNum,sex,IDCardNum,lastLogTime)"
                  " VALUES(:username,:password,:jobNum,:sex,:IDCardNum,:lastLogTime)");
    query.bindValue(":username",username);
    query.bindValue(":password",ui->pswLineEdit->text());
    query.bindValue(":jobNum",ui->jobNumLineEdit->text());
    query.bindValue(":sex",ui->sexComboBox->currentText());
    query.bindValue(":IDCardNum",ui->idCardLineEdit->text());
    query.bindValue(":lastLogTime",curDateTime);
    if(query.exec()) //添加成功
    {
        QMessageBox::information(this,tr("提示"),tr("用户添加成功"),QMessageBox::Ok);
        this->accept();
    }
    else
    {
        QMessageBox::critical(this,tr("严重错误"),tr("抱歉，用户添加失败。\n错误原因：")
                              + query.lastError().text());
    }
   // this->accept();
}

void AddUserDlg::paintEvent(QPaintEvent *event)
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

void AddUserDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void AddUserDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void AddUserDlg::mouseMoveEvent(QMouseEvent *event) //按下回车跳转到下一个控件
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}


void AddUserDlg::initEvent()    //为控件安装事件过滤器
{
    ui->usernameLineEdit->installEventFilter(this);
    ui->pswLineEdit->installEventFilter(this);
    ui->jobNumLineEdit->installEventFilter(this);
    ui->idCardLineEdit->installEventFilter(this);
    ui->sexComboBox->installEventFilter(this);
}

void AddUserDlg::setErrorLabelText(const QString &text, const QPoint &point)
{
    if(errorLabel->isVisible())     //避免出现动画还没stop，就按下第二次确定的重复动画
        return;
    errorLabel->setText(text);          //设置errorlabel的显示文本
    errorLabel->setGeometry(point.x(),point.y(),            //设置errorLabel的位置，以及根据文本确定宽度
                            fontMetrics().width(text) + 30,40);
    errorLabel->setVisible(true);          //设置label为可见
    QTimer::singleShot(500,this,SLOT(setLabelOpacity()));      //显示1秒后，执行隐藏动画
}

void AddUserDlg::on_closeBtn_clicked()
{
    this->reject();
}


void AddUserDlg::on_hidePswCheckBox_clicked(bool checked)
{
    if(checked)
        ui->pswLineEdit->setEchoMode(QLineEdit::Normal);
    else
        ui->pswLineEdit->setEchoMode(QLineEdit::Password);
}

void AddUserDlg::on_cancelBtn_clicked()
{
    this->reject();
}
