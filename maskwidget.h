#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include <QDialog>
class QLineEdit;
class QPushButton;
class QLabel;

class MaskWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MaskWidget(const QString &password,QWidget *parent = 0);
    void accept();
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *event);
private:
    QLineEdit *lineEdit;
    QLabel *inforLabel;
    QPushButton *okBtn;
    QLabel *label;
    QString password;
    QLabel *errorLabel;
};

#endif // MASKWIDGET_H
