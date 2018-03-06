
#include "navdelegate.h"

#include "navmodel.h"
#include "colordefines.h"

#include <QPainter>

NavDelegate::NavDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, m_pending(false)
{

}

NavDelegate::~NavDelegate()
{

}

QSize NavDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	NavModel::TreeNode* node = (NavModel::TreeNode*)index.data(Qt::UserRole).toUInt();

	if ( node->level == 1)
		return QSize(50, 35);
	else
		return QSize(50, 28);
}

void NavDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	NavModel::TreeNode* node = (NavModel::TreeNode*)index.data(Qt::UserRole).toUInt();

	painter->setRenderHint(QPainter::Antialiasing);

	// Fill background.
	if ( option.state & QStyle::State_Selected )
	{
		painter->fillRect(option.rect, color_selected);
	}
	else if ( option.state & QStyle::State_MouseOver )
	{
		painter->fillRect(option.rect, color_hover);
	}
	else
	{
		if ( node->level == 1 )
			painter->fillRect(option.rect, color_parent_normal);
		else
			painter->fillRect(option.rect, color_child_normal);
	}

	// draw decorate
	if ( node->children.size() != 0 )
	{
		QString imgPath;

		if ( node->collapse )
		{
			if ( option.state & QStyle::State_Selected )
                imgPath = ":/images/unexpand_selected.png";
			else
                imgPath = ":/images/unexpand_normal.png";
		}
		else
		{
			if ( option.state & QStyle::State_Selected )
                imgPath = ":/images/expand_selected.png";
			else
                imgPath = ":/images/expand_normal.png";
		}

		QPixmap img(imgPath);
		QRect targetRect = option.rect;
		targetRect.setWidth(16);
		targetRect.setHeight(16);
		QPoint c = option.rect.center();
		c.setX(8);
		targetRect.moveCenter(c);
		painter->drawPixmap(targetRect, imgPath, img.rect());
	}

	// draw text.
	QPen textPen( option.state & QStyle::State_Selected ? color_text_selected : color_text_normal);
	painter->setPen(textPen);

	int margin = 25;

	if ( node->level == 2 )
		margin = 45;

	QRect rect = option.rect;
	rect.setWidth(rect.width() - margin);
	rect.setX(rect.x() + margin);

	QFont normalFont("Microsoft Yahei", 9);
	painter->setFont(normalFont);

	painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString() );

	// draw line

	QPen linePen(color_line);
	linePen.setWidth(1);
	painter->setPen(linePen);

	if ( node->level == 1 
		|| (node->level == 2 && node->theLast ) )
	{
		painter->drawLine(
			QPointF(option.rect.x(), option.rect.y()+option.rect.height()-1), 
			QPointF(option.rect.x() + option.rect.width(), option.rect.y()+option.rect.height()-1));
	}


	// draw record count
	int recordCount = node->count;
	if ( m_pending || 0 >= recordCount)
		return;

	QPen decorationPen(option.state & QStyle::State_Selected ? color_selected : color_text_selected);
	QBrush decorationBrush(option.state & QStyle::State_Selected ? color_text_selected : color_selected);
	QFont decorationFont("Microsoft Yahei", 8);
	painter->setFont(decorationFont);

	QRect decoration = option.rect;
	decoration.setHeight(15);
	decoration.moveCenter(option.rect.center());
	decoration.setLeft(option.rect.right()-50);
	decoration.setRight(option.rect.right()-15);

	painter->setPen(decorationPen);
	QPainterPath path;
	path.addRoundedRect(decoration, 7, 7);
	painter->fillPath(path, decorationBrush);

	QString decorationText = QString("%1").arg(recordCount);
	if ( recordCount > 999 )
		decorationText = "999+";

	painter->drawText(decoration, Qt::AlignCenter | Qt::AlignVCenter, decorationText);
}
