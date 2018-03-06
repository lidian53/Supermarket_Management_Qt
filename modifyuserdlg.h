#ifndef MODIFYUSERDLG_H
#define MODIFYUSERDLG_H

#include <QDialog>

namespace Ui {
class ModifyUserDlg;
}

class ModifyUserDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModifyUserDlg(QString username, QWidget *parent = 0);
    ~ModifyUserDlg();
    
private slots:
    void on_cancelBtn_clicked();

    void on_okBtn_clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::ModifyUserDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动
    QString username;       //当前用户名

    void initView();
};

#endif // MODIFYUSERDLG_H
