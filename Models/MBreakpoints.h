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
#ifndef MBREAKPOINTS_H
#define MBREAKPOINTS_H

#include <QAbstractTableModel>
#include <QVariant>
#include "../Global.h"
#include "../Models/MDB.h"
#define NOMINMAX
#include <Windows.h>


Q_DECLARE_METATYPE(Breakpoint)
Q_DECLARE_METATYPE(BREAKPOINTTYPE)

class MBreakpoints : public QAbstractTableModel
 {
     Q_OBJECT

 public:
     enum Entries {OFFSET,SIZE,TYPE,ACTIVE};

     MBreakpoints(QObject *parent=0);
     MBreakpoints(QList<Breakpoint> bps, QObject *parent);
     int rowCount(const QModelIndex &parent) const;
     int columnCount(const QModelIndex &parent) const;

     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
     bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
     bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());


     /* custom function */
     const bool exists(address_t addr) const;
     const int row(address_t addr) const;
     const address_t address(int row) const;
     QList<Breakpoint> breakpoints() const;

public slots:
     void refresh();
private:
    QList<Breakpoint>  m_Breakpoints;

 };


#endif // MBREAKPOINTS_H
