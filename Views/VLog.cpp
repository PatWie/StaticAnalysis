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
#include "VLog.h"

VLog::VLog(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_Style = new HStyle;
    setFont(m_Style->font());
    setReadOnly(true);

}

void VLog::log(QString str)
{
    appendHtml(str);
    //appendPlainText(str);
    QTextCursor c =  textCursor();
    c.movePosition(QTextCursor::End);
    setTextCursor(c);
}

void VLog::log(QString str, QString str2, QString col){
    QString html = str + "<span style=\"color:"+col +";\">"+str2+"</span>";
    log(html);
}

void VLog::start(){
    emit onLogViewReady();
    log("start new instance...");
}




QSize VLog::sizeHint() const
{
    return QSize(50,m_Style->fontHeight()*6);
}
