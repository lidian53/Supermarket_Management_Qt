#ifndef GOODSNUMDELEGATE_H
#define GOODSNUMDELEGATE_H

#include <QItemDelegate>

class GoodsNumDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit GoodsNumDelegate(int selectColumn,QObject *parent = 0);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
    
public slots:
private slots:
    void commitAndCloseEditor();
private:
    int numColumn;
};

#endif // GOODSNUMDELEGATE_H
