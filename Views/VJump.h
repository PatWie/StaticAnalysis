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
#ifndef VJUMP_H
#define VJUMP_H

#include "../Helpers/HStyle.h"
#include "../Models/MDB.h"
#include "../Global.h"
#include <QAbstractScrollArea>
#include <QMap>
#include <QColor>


class VJump : public QAbstractScrollArea
{
    Q_OBJECT

    const int m_Breakpoint_radius;
    const int m_Breakpoint_space;

    HStyle* m_Style;
    address_t m_clickedaddress;

    CodeIter m_Start;
    int m_Lines;

    MDB* DB;

    QPainter *painter;
    CodeIter End;

    QMap<ELS,QColor> m_Colors;

public:

    explicit VJump(QWidget *parent = 0);
    QSize sizeHint() const;

    void repaint();
public slots:
    void setTopInstr(CodeIter Start);
    void setNumberOfVisibleLines(int lines);
    void setClickedLine(address_t line);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    const int middleOfLine(int Line) const;
    void drawJump(address_t StartAddress, CodeIter JumpInstruction, int Line);
    void drawLabel(int Line, QString Text);
    void drawBreakpoint(address_t StartAddress, CodeIter JumpInstruction, int Line);

signals:
    void forceRepaint();
};

#endif // VJUMP_H
