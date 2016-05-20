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
#include "HString.h"

#include <QString>
#include <QRegExp>

HString::HString()
{
}


QString HString::addrToString(const address_t addr)
{
    return QString("%1").arg(addr,8,16,QChar('0')).toUpper();
}



bool HString::isHex(QString Str){
    QRegExp hexMatcher("^[0-9a-fA-F]*h?$", Qt::CaseInsensitive);
    return hexMatcher.exactMatch(Str);
}

QString HString::asHex(const quint64 val)
{
    return inFormat(val);
}

QString HString::inFormat(const quint64 val, NUMBERFORMAT NF)
{
    switch(NF){
    default:
    case N_HEX:
        // 0,...,9 in hex is the same as in dec
        if(val<10 && val>=0)
            return QString("%1").arg(val,1,16,QChar('0')).toUpper();
        return QString("%1").arg(val,1,16,QChar('0')).toUpper()+"h";
    case N_DEC:
        // 0,...,9 in hex is the same as in dec
        if(val<10 && val>=0)
            return QString("%1").arg(val);
        return QString("%1").arg(val)+"d";
    case N_BIN:
        return QString("%1").arg(val,1,2,QChar('0')).toUpper()+"b";
    case N_OCT:
        return QString("%1").arg(val,1,8,QChar('0')).toUpper()+"o";
    case N_ASCII:
        return QString("'%1'").arg((char)val);
    }
}
