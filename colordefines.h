#ifndef COLORDEFINES_H
#define COLORDEFINES_H

#include <QColor>

// Title bar
const QColor color_title_bar(88, 115, 201);

// Navi bar
const QColor color_hover(209, 216, 240);
const QColor color_selected(133, 153, 216);
const QColor color_child_normal(239, 241, 250);
const QColor color_parent_normal(247, 249, 255);
const QColor color_line(214, 216, 224);
const QColor color_text_normal(58, 58, 58);
const QColor color_text_selected(255, 255, 255);

// Menu
const QColor color_menu_hover(224, 232, 241);

inline QString ColorToString(QColor color)
{
	return QString("rgb(%1,%2, %3)")
		.arg(color.red()).arg(color.green()).arg(color.blue());
}

#endif // COLORDEFINES_H