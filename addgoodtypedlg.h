#ifndef ADDGOODTYPEDLG_H
#define ADDGOODTYPEDLG_H

#include <QDialog>

namespace Ui {
class AddGoodTypeDlg;
}

class AddGoodTypeDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddGoodTypeDlg(QWidget *parent = 0);
    ~AddGoodTypeDlg();
    
private:
    Ui::AddGoodTypeDlg *ui;
     bool isDrag;
     QPoint startPoint;
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void on_cancelBtn_clicked();
    void on_okBtn_clicked();
    void on_closeBtn_clicked();
};

#endif // ADDGOODTYPEDLG_H
