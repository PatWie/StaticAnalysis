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
#ifndef GLOBAL_H
#define GLOBAL_H

#define NOMINMAX
#include <Windows.h>

#include <QMap>
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"
#include <QtGlobal>
#include <QString>
#include <QStringList>

enum NUMBERFORMAT{N_HEX=16,N_DEC=10,N_BIN=2,N_OCT=8,N_ASCII=0,N_UNKNOWN=-1};

enum ELS{T_INSTRUCTION,
         T_ADDRESS,
         T_HIGHLIGHTEDADDRESS,
         T_DELIMITER,
         T_LABEL,
         T_PREFIX,
         T_CHAR,
         T_REGISTER,
         T_SIZE,
         T_CONSTANT,
         T_VARIABLE,
         T_IMPORT,
         T_SEGMENT,
         T_TEXT,
         T_COMMENT,
         T_NORMALBACKGROUND,
         T_HIGHLIGHTEDBACKGROUND,
         T_DEFAULT,
         T_INACTIVEARROW,
         T_ACTIVEARROW,
         T_UNUSEDBREAKPOINT,
         T_ACTIVEBREAKPOINT,
         T_INACTIVEBREAKPOINT,
         T_BREAKPOINTBACKGROUND,
         T_UNKNOWN};

typedef quint32 address_t;
typedef unsigned char  byte_t;
typedef QMap<address_t,DISASM>::const_iterator CodeIter;
typedef QMap<address_t,address_t>::ConstIterator JmpIter;

struct APIData_t
{
    quint64 idataAddress;
    QString apiName;
    QString libraryName;
    quint64 apiOffset;
    QStringList Arguments;
    QString ReturnType;

};

enum BREAKPOINTTYPE{BP_SOFTWARE, BP_MEMORY, BP_HARDWARE };

struct Breakpoint{
    address_t address;
    DWORD  size;
    BREAKPOINTTYPE type;
    bool active;
    QByteArray originalBytes;
};



#endif // GLOBAL_H
