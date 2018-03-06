#ifndef COLLECTDLG_H
#define COLLECTDLG_H

#include <QDialog>

namespace Ui {
class collectDlg;
}

class collectDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit collectDlg(double pay,QWidget *parent = 0);
    ~collectDlg();
    void accept();
    double getPayPrice() const;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private slots:
    void on_collectLineEdit_textChanged(const QString &arg1);

private:
    Ui::collectDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动
};

#endif // COLLECTDLG_H
