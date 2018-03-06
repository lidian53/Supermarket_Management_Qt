#ifndef SUPPLIERDLG_H
#define SUPPLIERDLG_H

#include <QDialog>

namespace Ui {
class SupplierDlg;
}
class QSqlTableModel;
enum {
    Supplier_Id,
    Supplier_Name,
    Supplier_Contact,
    Supplier_Telephone,
    Supplier_Address,
    Supplier_Note
};

class SupplierDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit SupplierDlg(QWidget *parent = 0);
    ~SupplierDlg();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void on_delBtn_clicked();

    void on_addBtn_clicked();
    void copyTheNameSlot();
    void copyTheNumberSlot();

private:
    Ui::SupplierDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动
    QSqlTableModel *supplierModel;
    QAction *copyNameAction;
    QAction *copyNumberAction;

    void  initialisePage();
};

#endif // SUPPLIERDLG_H
