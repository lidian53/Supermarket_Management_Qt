#include "addsupplierdlg.h"
#include "ui_addsupplierdlg.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include "showinforlabel.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

AddSupplierDlg::AddSupplierDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSupplierDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    errorLabel = new ShowInforLabel(this);
    errorLabel->setVisible(false);
    ui->nameLineEdit->installEventFilter(this);
    ui->contactLineEdit->installEventFilter(this);
    ui->numberLineEdit->installEventFilter(this);
    ui->addressLineEdit->installEventFilter(this);
    ui->noteTextEdit->installEventFilter(this);
}

AddSupplierDlg::~AddSupplierDlg()
{
    delete ui;
}

bool AddSupplierDlg::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->nameLineEdit || target == ui->contactLineEdit || target ==ui->numberLineEdit
            || target == ui->addressLineEdit || target == ui->noteTextEdit)
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

void AddSupplierDlg::paintEvent(QPaintEvent *event)
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

void AddSupplierDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->pos().y() < 50)           //判断是否为左键
    {
        startPoint = event->globalPos() - frameGeometry().topLeft();
        isDrag = true;
        event->accept();
       // startPoint = QPoint(event->pos());          //将单击point存储
    }
}

void AddSupplierDlg::mouseReleaseEvent(QMouseEvent *event)
{
    isDrag = false;
}

void AddSupplierDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && event->pos().y() < 70 && isDrag)
    {
        move(event->globalPos() - startPoint);
        event->accept();
    }
}

void AddSupplierDlg::on_okBtn_clicked()     //确定添加按钮
{
    if(ui->nameLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入供应商名称"),QPoint(ui->nameLineEdit->pos() + QPoint(-12,10)));
        ui->nameLineEdit->setFocus();
        return;
    }
    else if(ui->contactLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入联系人"),QPoint(ui->contactLineEdit->pos() + QPoint(-12,10)));
        ui->contactLineEdit->setFocus();
        return;
    }
    else if(ui->numberLineEdit->text().isEmpty())
    {
        this->setErrorLabelText(tr("请输入联系电话"),QPoint(ui->numberLineEdit->pos() + QPoint(-12,10)));
        ui->numberLineEdit->setFocus();
        return;
    }
    QSqlQuery query;
    //先判断是否已经有了该供应商
    query.exec(tr("select name from supplierTb"));
    bool hasSupplier = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->nameLineEdit->text())
        {
            hasSupplier = true;
            break;
        }
    }
    if(hasSupplier) //如果已经有了该供应商
    {
        int r = QMessageBox::question(this,tr("警告"),tr("该供应商已经存在，是否继续添加？"),QMessageBox::No | QMessageBox::Yes);
        if(r == QMessageBox::No)
        {
            ui->nameLineEdit->selectAll();
            ui->nameLineEdit->setFocus();
            return;
        }
    }
    query.exec(tr("select *from supplierTb"));
    int id = 0;
    if(query.last())
    {
        id = query.value(0).toInt();
    }
    query.prepare(tr("insert into supplierTb values(:id,:name,:contact,:phone,:address,:note)"));
    query.bindValue(":id",++id);
    query.bindValue(":name",ui->nameLineEdit->text());
    query.bindValue(":contact",ui->contactLineEdit->text());
    query.bindValue(":phone",ui->numberLineEdit->text());
    QString addressText = ui->addressLineEdit->text();
    query.bindValue(":address",addressText.isEmpty()? tr("none") : addressText);
    QString noteText = ui->noteTextEdit->toPlainText();
    query.bindValue(":note",noteText.isEmpty() ? tr("none"):noteText);
    if(query.exec())
    {
        QMessageBox msgBox;
        msgBox.setText("添加成功！");
   //     msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowTitle(tr("提示"));
        msgBox.setButtonText(QMessageBox::Yes,tr("继续添加下一条"));
        msgBox.setButtonText(QMessageBox::No,tr("关闭"));
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes)     //继续添加下一条
        {
            ui->nameLineEdit->clear();
            ui->contactLineEdit->clear();
            ui->addressLineEdit->clear();
            ui->numberLineEdit->clear();
            ui->noteTextEdit->clear();
            ui->nameLineEdit->setFocus();
        }
        else
        {
            this->accept();
        }
    }
    else
    {
        QMessageBox::warning(this,tr("添加供应商"),tr("添加供应商失败。\n原因：%1")
                             .arg(query.lastError().text()));
    }
}

void AddSupplierDlg::setLabelOpacity()
{
    QPropertyAnimation *animation = new QPropertyAnimation(errorLabel,"opacity");

    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setDuration(1500);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation,SIGNAL(finished()),errorLabel,SLOT(setWindowVisable()));//当结束时，设置label为不可见
}

void AddSupplierDlg::setErrorLabelText(const QString &text, const QPoint &point)
{
    if(errorLabel->isVisible())     //避免出现动画还没stop，就按下第二次确定的重复动画
        return;
    errorLabel->setText(text);          //设置errorlabel的显示文本
    errorLabel->setGeometry(point.x(),point.y(),            //设置errorLabel的位置，以及根据文本确定宽度
                            fontMetrics().width(text) + 30,40);
    errorLabel->setVisible(true);          //设置label为可见
    QTimer::singleShot(500,this,SLOT(setLabelOpacity()));      //显示1秒后，执行隐藏动画
}
