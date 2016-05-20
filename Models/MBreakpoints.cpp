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
#include "MBreakpoints.h"
#include "../Helpers/HString.h"
#include "../tia.h"

/***************** default constructors ********************************/
MBreakpoints::MBreakpoints(QObject *parent)
    : QAbstractTableModel(parent)
{

}
MBreakpoints::MBreakpoints(QList<Breakpoint> bps, QObject *parent): QAbstractTableModel(parent)
{
    m_Breakpoints = bps;
}
int MBreakpoints::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Breakpoints.count();
}
int MBreakpoints::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}
QVariant MBreakpoints::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case OFFSET:
            return tr("Offset");

        case SIZE:
            return tr("Size");

        case TYPE:
            return tr("Type");

        case ACTIVE:
            return tr("Active");

        default:
            return QVariant();
        }
    }
    return QVariant();
}



QVariant MBreakpoints::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_Breakpoints.count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {

        Breakpoint el = m_Breakpoints[index.row()];

        switch(index.column()){
        case OFFSET:
            return HString::addrToString(el.address);
        case SIZE:
            return (uint)el.size;
        case TYPE:
            return el.type;
        case ACTIVE:
            return el.active;
        }


    }
    return QVariant();
}

bool MBreakpoints::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        Breakpoint bp;
        m_Breakpoints.insert(position,bp);
    }

    endInsertRows();
    return true;
}


bool MBreakpoints::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        m_Breakpoints.removeAt(position);
    }

    endRemoveRows();
    return true;
}

const bool MBreakpoints::exists(address_t addr) const
{
    foreach(Breakpoint bp, m_Breakpoints){
        if(bp.address == addr)
            return true;
    }
    return false;
}

const int MBreakpoints::row(address_t addr) const
{
    int pos=0;
    foreach(Breakpoint bp, m_Breakpoints){
        if(bp.address == addr)
            return pos;
        pos++;
    }
    return -1;
}

const address_t MBreakpoints::address(int row) const
{
    return m_Breakpoints[row].address;
}

QList<Breakpoint> MBreakpoints::breakpoints() const
{
    return m_Breakpoints;
}


bool MBreakpoints::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        Breakpoint p = m_Breakpoints.value(row);

        switch(index.column()){
        case OFFSET:
            p.address = value.value<address_t>();
            break;
        case SIZE:
            p.size = value.value<DWORD>();
            break;

        case TYPE:
            p.type = value.value<BREAKPOINTTYPE>();
            break;

        case ACTIVE:
            p.active = value.value<bool>();
            break;
        default:
            return false;
        }
        m_Breakpoints.replace(row,p);
        emit(dataChanged(index, index));

        return true;
    }

    return false;


}

Qt::ItemFlags MBreakpoints::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

void MBreakpoints::refresh(){
    emit layoutChanged();
}


