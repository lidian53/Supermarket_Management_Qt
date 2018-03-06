#include "maskwidget.h"
#include<QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QApplication>

MaskWidget::MaskWidget(const QString &password, QWidget *parent) :
    QDialog(parent)
{
    this->password = password;
    lineEdit = new QLineEdit(this);
    lineEdit->setEchoMode(QLineEdit::Password);
    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(accept()));  //按下回车时，与点击确定一样
    label = new QLabel(tr("请输入密码："),this);
    label->setStyleSheet(tr("color:white;font:bold 16pt;"));
    inforLabel = new QLabel(tr("锁定中..."),this);
    inforLabel->setStyleSheet(tr("color:white;font:bold 25pt;"));
    okBtn = new QPushButton(tr("提交"),this);
    okBtn->setDefault(true);
    connect(okBtn,SIGNAL(clicked()),this,SLOT(accept()));
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet(tr("color:red;font:bold 14pt;"));
    errorLabel->setVisible(false);
    errorLabel->resize(QSize(250,30));
  /*  QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(okBtn);

    this->setLayout(layout); */
  //  this->setAutoFillBackground(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QString style = QString("QPushButton { border: 2px solid #8f8f91;border-radius: 6px;"
                            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #f6f7fa, stop: 1 #dadbde);"
                            "min-height:20px; min-width:50px;}"
                            "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                            "stop: 0 #dadbde, stop: 1 #f6f7fa); }"
                            " QPushButton:pressed { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                              "stop: 0 #dadbde, stop: 1 #f6f7fa);}"
                            " QLineEdit {color: rgb(127, 0, 63);background-color: rgb(255, 255, 241); selection-color: white;"
                            "selection-background-color: rgb(191, 31, 127);border: 2px groove gray;"
                            "border-radius: 10px; padding: 2px 4px; }");
    this->setStyleSheet(style);
    lineEdit->setFocus();
}

void MaskWidget::accept()   //
{
    int width = this->width();
    int height = this->height();
    if(this->lineEdit->text().isEmpty())
    {
        QApplication::beep();
        errorLabel->setText(tr("请输入密码..."));
        errorLabel->move(width / 2 - 70,height /2 + 150);
        errorLabel->setVisible(true);
        this->lineEdit->setFocus(); //设置焦点
    }
    else if(lineEdit->text() != password)       //如果密码不正确
    {
        QApplication::beep();
        errorLabel->setText(tr("密码不正确，请重新输入..."));
        errorLabel->move(width / 2 - 70,height /2 + 150);
        errorLabel->setVisible(true);
        this->lineEdit->selectAll();
        this->lineEdit->setFocus();
    }
    else
    {
        QDialog::accept();
    }
}

void MaskWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(),QColor(0,0,0,100));
}

void MaskWidget::resizeEvent(QResizeEvent *)
{
    int width = this->width();
    int height = this->height();
    inforLabel->move(width / 2- 100,height / 2);
    label->move(width / 2 - 200,height /2 + 100);
    lineEdit->move(width / 2 - 70,height /2+ 100);
    okBtn->move(width / 2 + 60,height /2 + 100);
}

void MaskWidget::showEvent(QShowEvent *event)   //显示时触发，为了清空文本框中原来的密码
{
    this->lineEdit->clear();
    this->lineEdit->setFocus();
    this->errorLabel->clear();
}
