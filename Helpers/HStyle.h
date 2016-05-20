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
#ifndef HSTYLE_H
#define HSTYLE_H

#include <QFont>
#include "../Global.h"

class HStyle
{
    int m_FontWidth;
    int m_FontHeight;
    QFont m_DefaultFont;
public:
    HStyle();
    const int fontWidth() const;
    const int fontHeight() const;
    const QFont font() const;
    const QMap<ELS, QColor> colors() const;
};

#endif // HSTYLE_H
