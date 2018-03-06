#ifndef CHOICEGOODSDLG_H
#define CHOICEGOODSDLG_H

#include <QDialog>
class QModelIndex;
class QSqlTableModel;
class QStringListModel;
namespace Ui {
class ChoiceGoodsDlg;
}

class ChoiceGoodsDlg : public QDialog
{
    Q_OBJECT
    
public:
    enum GoodsNameEnum {
        GoodsName_Id,
        GoodsName_TypeId,
        GoodsName_Name,
        GoodsName_Stock,
        GoodsName_Bid,
        GoodsName_Retail,
        GoodsName_Format,
        GoodsName_Unit,
        GoodsName_Note
    };
    explicit ChoiceGoodsDlg(QWidget *parent = 0);
    ~ChoiceGoodsDlg();
    QString getChoiceGoodsNum() const;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
    void accept();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_listView_clicked(const QModelIndex &index);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::ChoiceGoodsDlg *ui;
    void loadList();
    QPoint startPoint;
    bool isDrag;            //是否拖动
    QString goodsNum;
    QSqlTableModel *model;
    QStringListModel *typeModel;
};

#endif // CHOICEGOODSDLG_H
