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
#include "HArrow.h"

#include <QPainter>
#include <QColor>
#include <QPainterPath>
#include <QPointF>

HArrow::HArrow()
{
}

void HArrow::drawStraightArrow(QPainter *painter, int x1, int y1, int x2, int y2)
{


    painter->drawLine(x1,y1,x2,y2);



    /*
    // this does not work
    QPainterPath path;

    path.moveTo( QPointF(x2, x2));
    path.lineTo (QPointF(x2-ArrowSizeX,y2-ArrowSizeY));
    path.lineTo (QPointF(x2-ArrowSizeX,y2+ArrowSizeY));
    path.closeSubpath();

    painter->setPen (Qt :: NoPen);
    painter->fillPath (path, QBrush (color));*/



    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2+ArrowSizeY);

}

void HArrow::drawToRightArrowTip(QPainter* painter, int x2, int y2){
    painter->save();
    QPen tmp = painter->pen();
    tmp.setStyle(Qt::SolidLine);
    tmp.setWidth(2);
    painter->setPen(tmp);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2+ArrowSizeY);
    painter->restore();
}

void HArrow::drawToBottomArrowTip(QPainter* painter, int x2, int y2){
    painter->save();
    QPen tmp = painter->pen();
    tmp.setStyle(Qt::SolidLine);
    tmp.setWidth(2);
    painter->setPen(tmp);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2+ArrowSizeX,y2-ArrowSizeY);
    painter->restore();
}

void HArrow::drawToTopArrowTip(QPainter* painter, int x2, int y2){
    painter->save();
    QPen tmp = painter->pen();
    tmp.setStyle(Qt::SolidLine);
    tmp.setWidth(2);
    painter->setPen(tmp);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2+ArrowSizeY);
    painter->drawLine(x2,y2,x2+ArrowSizeX,y2+ArrowSizeY);
    painter->restore();
}

void HArrow::drawJumpArrowInViewport(QPainter* painter,  int x_start, int y_start,int y_end, int Offset)
{
    /*
     * draws
     *
     * +----     start
     * |
     * |         middle
     * |
     * +--->     end
     */




    // start
    painter->drawLine(x_start,y_start,x_start-Offset,y_start);
    // middle
    painter->drawLine(x_start-Offset,y_start,x_start-Offset,y_end);
    // end
    painter->drawLine(x_start,y_end,x_start-Offset,y_end);
    drawToRightArrowTip(painter,x_start,y_end);



}

void HArrow::drawJumpArrowLeaveTopViewport(QPainter *painter, int x_start, int y_start, int Offset)
{
    drawToTopArrowTip(painter,x_start-Offset,4);
    // start
    painter->drawLine(x_start,y_start,x_start-Offset,y_start);

    // middle
    painter->drawLine(x_start-Offset,y_start,x_start-Offset,4);
}
void HArrow::drawJumpArrowEntryTopViewport(QPainter *painter, int x_start, int y_start, int Offset)
{

    // start
    painter->drawLine(x_start,y_start,x_start-Offset,y_start);
    drawToRightArrowTip(painter,x_start,y_start);
    // middle
    painter->drawLine(x_start-Offset,y_start,x_start-Offset,0);
}

void HArrow::drawJumpArrowLeaveBottomViewport(QPainter *painter,int x_start, int y_start, int Offset, int viewportheight)
{

    // start
    painter->drawLine(x_start,y_start,x_start-Offset,y_start);
    // middle
    painter->drawLine(x_start-Offset,y_start,x_start-Offset,viewportheight);

    drawToBottomArrowTip(painter,x_start-Offset,viewportheight);
}

void HArrow::drawJumpArrowEntryBottomViewport(QPainter *painter, int x_start, int y_start, int Offset, int viewportheight)
{
    // start
    drawToRightArrowTip(painter,x_start,y_start);
    painter->drawLine(x_start,y_start,x_start-Offset,y_start);
    // middle
    painter->drawLine(x_start-Offset,y_start,x_start-Offset,viewportheight);


}
