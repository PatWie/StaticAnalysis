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
#include "HStyle.h"

#include <QFontMetrics>
#include <QMap>
#include <QColor>
#include <QString>

HStyle::HStyle()
{
    m_DefaultFont = QFont("Consolas", 10);  //m_DefaultFont = QFont("Fixedsys", 10);

    const QFontMetrics metrics(m_DefaultFont);
    // it is a monospace font, so we us 'W' to get the character width
    m_FontWidth  = metrics.width('W');
    m_FontHeight = metrics.height();
}

const int HStyle::fontWidth() const
{
    return m_FontWidth;
}

const int HStyle::fontHeight() const
{
    return m_FontHeight;
}

const QFont HStyle::font() const
{
    return m_DefaultFont;
}

const QMap<ELS, QColor> HStyle::colors() const
{

    QMap<ELS,QColor> m_Colors;

    m_Colors.insert(T_ADDRESS,QColor("#ae81ff"));
    m_Colors.insert(T_INSTRUCTION,QColor("#34A2D9"));
    m_Colors.insert(T_PREFIX,QColor("#34A2D9"));

    m_Colors.insert(T_DELIMITER,QColor("#ff0000"));
    m_Colors.insert(T_CHAR,QColor("#ff0000"));
    m_Colors.insert(T_CONSTANT,QColor("#F27709"));
    m_Colors.insert(T_SEGMENT,QColor("#ffffff"));
    m_Colors.insert(T_LABEL,QColor("#aaaa70"));
    m_Colors.insert(T_REGISTER,QColor("#00aa61"));
    m_Colors.insert(T_SIZE,QColor("#ffffff"));
    m_Colors.insert(T_VARIABLE,QColor("#FFF39E"));
    m_Colors.insert(T_IMPORT,QColor("#f92672"));
    m_Colors.insert(T_TEXT,QColor("#ffffff"));

    m_Colors.insert(T_HIGHLIGHTEDADDRESS,QColor("#bdaeec"));
    m_Colors.insert(T_NORMALBACKGROUND,QColor("#272822"));
    m_Colors.insert(T_BREAKPOINTBACKGROUND,QColor("#460306"));
    m_Colors.insert(T_HIGHLIGHTEDBACKGROUND,QColor("#57594c"));
    m_Colors.insert(T_DEFAULT,QColor("#ffffff"));

    m_Colors.insert(T_COMMENT,QColor("#a6e22e"));
    m_Colors.insert(T_INACTIVEARROW,QColor("#888861"));
    m_Colors.insert(T_ACTIVEARROW,QColor("#66d9ef"));

    m_Colors.insert(T_UNUSEDBREAKPOINT,QColor("#888861"));
    m_Colors.insert(T_INACTIVEBREAKPOINT,QColor(0,85,0));
    m_Colors.insert(T_ACTIVEBREAKPOINT,Qt::red);


    return m_Colors;
}
