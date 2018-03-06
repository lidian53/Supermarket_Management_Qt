#ifndef ADDSUPPLIERDLG_H
#define ADDSUPPLIERDLG_H

#include <QDialog>

namespace Ui {
class AddSupplierDlg;
}
class ShowInforLabel;

class AddSupplierDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddSupplierDlg(QWidget *parent = 0);
    ~AddSupplierDlg();
    bool eventFilter(QObject *target, QEvent *event);
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
private slots:
    void on_okBtn_clicked();
    void setLabelOpacity();

private:
    Ui::AddSupplierDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动
    ShowInforLabel *errorLabel;
    void setErrorLabelText(const QString &text,const QPoint &point = QPoint(80,95));
    bool hasTheSupplier(const QString);
};

#endif // ADDSUPPLIERDLG_H
