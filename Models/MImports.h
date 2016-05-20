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
#ifndef MImports_H
#define MImports_H

#include <QAbstractTableModel>
#include <QVariant>

#include "../Global.h"


class MImports : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Entries {ADDRESS,NAME,LIBRARY,IDATA};
    MImports(QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;



public slots:
    void refresh();
private:


};

#endif // MImports_H
