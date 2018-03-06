#include "goodsnumdelegate.h"
#include <QSpinBox>

GoodsNumDelegate::GoodsNumDelegate(int selectColumn, QObject *parent) :
    QItemDelegate(parent)
{
    numColumn = selectColumn;
}

void GoodsNumDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == numColumn)
    {
        int num = index.model()->data(index,Qt::DisplayRole).toInt();
        QSpinBox *spinBox =  qobject_cast<QSpinBox *>(editor);
        spinBox->setValue(num);
    }
    else
    {
        QItemDelegate::setEditorData(editor,index);
    }
}

void GoodsNumDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == numColumn)
    {
        QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);
        model->setData(index,spinBox->value());
    }
    else
    {
        QItemDelegate::setModelData(editor,model,index);
    }
}

QWidget *GoodsNumDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == numColumn)
    {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setRange(1,99);
        connect(spinBox,SIGNAL(editingFinished()),this,SLOT(commitAndCloseEditor()));
        return spinBox;
    }
    else
    {
        return QItemDelegate::createEditor(parent,option,index);
    }
}

void GoodsNumDelegate::commitAndCloseEditor()
{
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(sender());
    emit commitData(spinBox);
    emit closeEditor(spinBox);
}
