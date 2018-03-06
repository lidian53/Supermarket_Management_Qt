#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include <QAbstractItemView>

class HistogramView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit HistogramView(QWidget *parent = 0);
    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint);
    QModelIndex indexAt(const QPoint &point) const;
    void setSelectionModel(QItemSelectionModel *selectionModel);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset() const;
    int verticalOffset()const;
    bool isIndexHidden(const QModelIndex &index) const;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
    QItemSelectionModel *selections;
    QList<QRegion> MRegionList;
    QList<QRegion> CRegionList;
    QVector<double> totalPrice;
};

#endif // HISTOGRAMVIEW_H
