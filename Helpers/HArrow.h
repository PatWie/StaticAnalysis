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
#ifndef HARROW_H
#define HARROW_H

#include <QPainter>
#include <QColor>

/**
 * @brief helper for drawing arrows in all directions
 */
class HArrow
{
    static const int ArrowSizeX = 4;  // width  of arrow tip in pixel
    static const int ArrowSizeY = 4;  // height of arrow tip in pixel

public:
    HArrow();

    // draws "--->"
    static void drawStraightArrow(QPainter* painter, int x1, int y1, int x2, int y2);
    // draws ">"
    static void drawToRightArrowTip(QPainter *painter, int x2, int y2);
    // draws jump arrows that starts and ends within viewport
    static void drawJumpArrowInViewport(QPainter* painter,  int x_start, int y_start, int y_end, int Offset);
    // draws jump arrows that starts in viewport and leaves viewport
    static void drawJumpArrowLeaveTopViewport(QPainter* painter,  int x_start, int y_start,int Offset);
    // draws jump arrow that comes from outside the viewport (top)
    static void drawJumpArrowEntryTopViewport(QPainter *painter, int x_start, int y_start, int Offset);

    static void drawJumpArrowLeaveBottomViewport(QPainter* painter,    int x_start, int y_start,int Offset,int viewportheight);
    static void drawJumpArrowEntryBottomViewport(QPainter* painter,    int x_start, int y_start,int Offset,int viewportheight);
    // draws "v"
    static void drawToBottomArrowTip(QPainter *painter, int x2, int y2);
    // draws "^"
    static void drawToTopArrowTip(QPainter *painter, int x2, int y2);

};

#endif // HARROW_H
