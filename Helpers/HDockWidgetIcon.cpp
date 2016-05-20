/*
 ____  __   __
(_  _)(  ) / _\
  )(   )( /    \
 (__) (__)\_/\_/

Copyright (C) 2014 mail@patwie.com (http://patwie.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "HDockWidgetIcon.h"


#include <QPainter>
HDockWidgetIcon::HDockWidgetIcon(const QIcon &icon, QStyle *style)
    : QProxyStyle(style)
    , icon_(icon)
{}


HDockWidgetIcon::~HDockWidgetIcon()
{}


void HDockWidgetIcon::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // cf. http://stackoverflow.com/questions/2386527/how-to-display-icon-in-qdockwidget-title-bar
    if(element == QStyle::CE_DockWidgetTitle)
    {
        //width of the icon
        int width = pixelMetric(QStyle::PM_ToolBarIconSize);
        //margin of title from frame
        int margin = baseStyle()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);

        QPoint icon_point(margin + option->rect.left(), margin + option->rect.center().y() - width/2);

        painter->drawPixmap(icon_point, icon_.pixmap(width, width));

        const_cast<QStyleOption*>(option)->rect = option->rect.adjusted(width, 0, 0, 0);
    }
    baseStyle()->drawControl(element, option, painter, widget);
}
