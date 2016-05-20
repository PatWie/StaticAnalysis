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
#ifndef HSTRING_H
#define HSTRING_H

#include "../Global.h"

#include <QString>

class HString
{
public:
    HString();

    /**
     * @brief converts an address into a hex string (with filling '0')
     * @param addr
     * @return address a shex string
     */
    static QString addrToString(const address_t addr);
    /**
     * @brief check (using RegEx) if a given string is a valid hex string
     * @param Str
     * @return valid?
     */
    static bool isHex(QString Str);
    /**
     * @brief converts integer value to hex string without filling '0'
     * @param val
     * @return
     */
    static QString asHex(const quint64 val);
    /**
     * @brief converts a given number into a another number system
     * @param val value to convert
     * @param NF target number system (see global.h)
     * @return
     */
    static QString inFormat(const quint64 val, NUMBERFORMAT NF=N_HEX);
};

#endif // HSTRING_H
