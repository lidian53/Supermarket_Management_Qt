#ifndef LOGINDLG_H
#define LOGINDLG_H


/*
登录窗口
*/
#include <QDialog>
class ShowInforLabel;

namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();
    QString getUserName() const;
private slots:
    void on_loginBtn_clicked();
    void setLabelOpacity();
    void on_checkBox_clicked(bool checked);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);


private:
    Ui::LoginDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动
    ShowInforLabel *errorLabel;
    void setErrorLabelText(const QString &text,const QPoint &point = QPoint(80,95));
    void readSetting();
    void writeSetting();
    void accept();
};

#endif // LOGINDLG_H
