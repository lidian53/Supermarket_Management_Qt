#ifndef ADDUSERDLG_H
#define ADDUSERDLG_H

#include <QDialog>

namespace Ui {
class AddUserDlg;
}
class ShowInforLabel;

class AddUserDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddUserDlg(QWidget *parent = 0);
    ~AddUserDlg();
    bool eventFilter(QObject *target, QEvent *event);
    
private slots:
    void setLabelOpacity(); //
    void on_okBtn_clicked();
    void on_closeBtn_clicked();

    void on_hidePswCheckBox_clicked(bool checked);

    void on_cancelBtn_clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    Ui::AddUserDlg *ui;
    QPoint startPoint;
    bool isDrag;            //是否拖动

    void initEvent();
    ShowInforLabel *errorLabel;
    void setErrorLabelText(const QString &text,const QPoint &point = QPoint(80,95));
};

#endif // ADDUSERDLG_H
