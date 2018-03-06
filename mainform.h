#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QModelIndex>

namespace Ui {
class MainForm;
}
class QComboBox;
class QPushButton;
class QSqlRelationalTableModel;
class QTableView;
class QTableWidget;
class QAction;
class QTableWidgetItem;
class QSqlTableModel;
class QSqlQueryModel;
class MaskWidget;
class NavModel;
class HistogramView;

class MainForm : public QWidget
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
    enum Sell {
        Sell_Id,
        Sell_Man,
        Sell_DateTime,
        Sell_FactPrice,
        Sell_CollectPrice,
        Sell_Count
    };

    enum SellList {
        SellList_Num,
        SellList_Id,
        SellList_TodayCount,
        SellList_GoodsId,
        SellList_Count,
        SellList_DateTime
    };
    enum SubmitState {
        Submit_Update,
        Submit_Insert
    };
    enum Supplier {
        Supplier_Id,
        Supplier_Name,
        Supplier_Contact,
        Supplier_Telephone,
        Supplier_Address,
        Supplier_Note
    };
    enum Purchase {
        Purchase_Id,
        Purchase_DateTime,
        Purchase_SupplierId,
        Purchase_TotalPrice,
        Purchase_Handelr,
        Purchase_Note
    };
    enum PurchaseDetail {
        PurDetail_Id,
        PurDetail_PurchaseId,
        PurDetail_GoodsId,
        PurDetail_Count
    };
    //用户枚举
    enum User {
        User_Name,
        User_Password,
        User_JobNum,
        User_Sex,
        User_IDCard,
        User_LastLoginTime
    };

    explicit MainForm(QString username,QWidget *parent = 0);
    ~MainForm();
    bool eventFilter(QObject *target, QEvent *event);
    void showName();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
private slots:
    void showGoodsInforSlot();
    void showSellPageSlot();
    void showSallStatisticsSlot();  //销售统计报表
    void showUserManagementSlot();      //用户管理界面
    void on_barcodeLineEdit_S_returnPressed();
    void completerChange(const QString &arg);
    void destoryCompleter();
    void goodsListDoubleClick(QModelIndex);        //选择商品双击
    void deleteSellListSlot();
    void updateSellPrice();
    void updateModel(QTableWidgetItem *item);
    void on_delSellGoodsBtn_S_clicked();
    void on_collectionsBtn_S_clicked();
    void on_sellIdLineEdit_S_lostFocus();
    void on_sellIdLineEdit_S_returnPressed();
    void showHistoryBillSlot();
    //历史账单
    void updateBillPageSlot();
    void on_startDateTimeEdit_B_dateTimeChanged(const QDateTime &date);
    void on_endDateTimeEdit_B_dateTimeChanged(const QDateTime &date);
    void on_sellListTableView_B_clicked(const QModelIndex &index);
    void on_searchForIdLineEdit_B_textChanged(const QString &arg1);
    void on_exportToExcelBtn_B_clicked();
    void on_clearSellListBtn_S_clicked();
    void on_delBillBtn_B_clicked();
    void on_sellManComboBox_B_currentIndexChanged(const QString &arg1);
    void on_goodsTypeComboBox_M_currentIndexChanged(const QString &arg1);
    void on_goodsNameDelBtn_M_clicked();
    void on_goodsNameTableView_M_clicked(const QModelIndex &index);
    void setWidgetStatus_M(bool isEnable);
    void on_addGoodsBtn_M_clicked();
    void on_submitBtn_M_clicked();
    void on_reviseBtn_M_clicked();
    void on_searchLineEdit_M_textChanged(const QString &arg1);
 //   void on_purchaseBtn_P_clicked();
    void purchaseDetailSlot();        //进货明细
    void on_addPurchase_P_clicked();
    //进货
    void copyPurchaseId();      //右键复制单号
    void on_purchaseIdLineEdit_P_textChanged(const QString &arg1);
    void on_startDateEdit_P_dateChanged(const QDate &date);
    void on_endDateEdit_P_dateChanged(const QDate &date);
    void on_purchaseTableView_P_clicked(const QModelIndex &index);
    void showSupplierSlot();
    void clickInfor(QString text);
    void on_AddBtn_U_clicked();
    void on_DeleteBtn_U_clicked();
    void on_ModifyBtn_U_clicked();

    void on_okBtn_S_clicked();

private:
    void showCurrentTime();
    void setUserLastLoginTime();
    void installWidgetEventFilter();
    void createPage();
    Ui::MainForm *ui;
    NavModel* model;

    QString userName;
    QPoint startPoint;
    bool isDrag;            //是否拖动
    int curTimeId;
    MaskWidget *maskWidget;
    //商品列表page

    QSqlRelationalTableModel *goodsNameModel_M;
    QSqlTableModel *UserManagementModel_U;    //用户管理的model
    QSqlQueryModel *comboBoxmodel_M;
    QTableView *goodsNameView;
    QTableView *goodsListWidget;
    QAction *delAction;
    QAction *addAction;
    QAction *reduceAction;
    double sumOfPrice;
    QString sellIdStr;
    int countOfSell;
    SubmitState submitState;
    void showGoodsListWidget();
    void produceSellID();           //设置单号
    void setSellCount();

    //历史账单
    QSqlTableModel *billModel_B;
    QSqlRelationalTableModel *billListModel_B;

    //进货
    QAction *copyAction_P;
    QSqlRelationalTableModel *purchaseModel_P;
    QSqlRelationalTableModel *purchaseDetailModel_P;
    void updatePurchaseView();      //更新进货界面的进货单号，总价等信息
    void purchase();          //主界面的进货按钮

    //统计报表
    HistogramView *histogramView;

    //添加商品类别
    void addGoodsType();
};

#endif // MAINFORM_H
