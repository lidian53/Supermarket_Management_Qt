#include "histogramview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QSqlQuery>
#include <QDate>

HistogramView::HistogramView(QWidget *parent) :
    QAbstractItemView(parent)
{
    QSqlQuery query;
    double temp = 0.0;      //每一天的总价
    totalPrice.clear();
    QDate date = QDate::currentDate().addDays(-7);
    QString  dateStr = date.toString("yyyy-MM-dd"); //一个星期前
    for(int i = 0;i < 7;i++)
    {
        query.exec(tr("select collectPrice from sellTb where sellDateTime like '%1%'")
                   .arg(dateStr));
        while(query.next())
        {
            temp += query.value(0).toDouble();
        }
        totalPrice.append(temp);
        date = date.addDays(1);
        dateStr = date.toString("yyyy-MM-dd");
        temp = 0.0;
    }
    //测试用的数据
    totalPrice.clear();
    for(int i = 1;i <= 7;++i)
        totalPrice.append(double(i * 10));
}

QModelIndex HistogramView::indexAt(const QPoint &point) const
{
    return QModelIndex();
}

void HistogramView::setSelectionModel(QItemSelectionModel *selectionModel)
{

}

void HistogramView::paintEvent(QPaintEvent *event)  //绘制柱状图 859 * 519
{
    /*
    *viewport宽 ： 859 高： 519
    *标尺x轴分别为100 以及 759 ，每一条标尺间相隔140
    *前面数字的x轴为50
    *标题的起始坐标为(350,50)
    *
    *
    *
    *
    *

    */
    QPainter painter(this->viewport());
   // painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen(Qt::gray,1);
    painter.setPen(pen);
    for(int i = 0;i < 10;i++) {
        painter.drawLine(QPoint(100,100 + 40 * i),QPoint(759,100 + 40 * i));
    }
    painter.setFont(QFont(tr("黑体"), 14, QFont::Bold));
    pen.setColor(Qt::black);
    painter.setPen(pen);
    for(int i = 0;i < 10;i++) {
        painter.drawText(QPoint(50,110 + 40 * i),tr("%1").arg(i * 100));
    }
    painter.drawText(QRectF(350,50,500,50),tr("最近一个月销售情况"));

    //绘制柱状图
    QColor color(79,129,189);
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
//    for(int i = 0;i < 10;i++) {
        painter.drawRect(100,100,30,500);
        painter.drawRect(QRect());
  //  }

}

//数据发生改变
void HistogramView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft,bottomRight);
    this->viewport()->update();
}

//选择发生改变
void HistogramView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    this->viewport()->update();
}

//单击柱状图
void HistogramView::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);
    setSelection(QRect(event->x(),event->y(),1,1),QItemSelectionModel::SelectCurrent);
}

void HistogramView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    int rows = model()->rowCount(this->rootIndex());
    int columns = model()->columnCount(this->rootIndex());
    QModelIndex selectedIndex;

}


//不需实现，但需要定义
QRegion HistogramView::visualRegionForSelection(const QItemSelection &selection) const{}

int HistogramView::horizontalOffset() const{}

int HistogramView::verticalOffset() const {}

bool HistogramView::isIndexHidden(const QModelIndex &index) const {}

QRect HistogramView::visualRect(const QModelIndex &index) const {}

void HistogramView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint){}

QModelIndex HistogramView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers){}
