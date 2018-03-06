#ifndef ADDPURCHASEDLG_H
#define ADDPURCHASEDLG_H

#include <QDialog>

namespace Ui {
class AddPurchaseDlg;
}
class QCompleter;
class QSqlQueryModel;
class QTableWidgetItem;

class AddPurchaseDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddPurchaseDlg(QString handlerName,QWidget *parent = 0);
    ~AddPurchaseDlg();
    bool eventFilter(QObject *target, QEvent *event);
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void on_addBtn_clicked();

    void on_delBtn_clicked();

    void on_purchaseIdLineEdit_editingFinished();
    void updateModel(QTableWidgetItem *item);

    void on_okBtn_clicked();

private:
    Ui::AddPurchaseDlg *ui;
    bool isDrag;
    QString handler;
    QPoint startPoint;
    QCompleter *completer;
    QString purchaseId;
    QSqlQueryModel *supplierModel;
    void  initialisePage();
    void createComploeter();
    void selectGoods();
    void addToTable(const QString &num);
    void updatePrice();

};

#endif // ADDPURCHASEDLG_H
