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
#ifndef MPATCHES_H
#define MPATCHES_H

#include <QWidget>
#include "../Global.h"

class MPatches : public QWidget
{
    Q_OBJECT
public:
    explicit MPatches(QWidget *parent = 0);

    void changeJumpType(address_t addr, byte_t newBytes );

signals:

public slots:

};

#endif // MPATCHES_H
