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
#include "MSegments.h"
#include "../tia.h"
#include "../Helpers/HString.h"

MSegments::MSegments(QObject *parent) :
    QAbstractTableModel (parent)
{

}

int MSegments::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TIA::instance()->m_Target->sections().count();
}

int MSegments::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}
QVariant MSegments::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= TIA::instance()->m_Target->sections().count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        PESectionData el = TIA::instance()->m_Target->sections().at(index.row());
        quint64 ModulBase = 0x00400000;

        switch(index.column()){
        case STARTADDRESS:
            return HString::addrToString(ModulBase + el.VirtualAddress);
        case ENDADDRESS:
            return HString::addrToString(ModulBase + el.VirtualAddress + el.VirtualSize);
        case NAME:
            return el.SectionName;

        }

    }

    return QVariant();
}


QVariant MSegments::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case NAME:
            return tr("Name");

        case STARTADDRESS:
            return tr("Start");

        case ENDADDRESS:
            return tr("End");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags MSegments::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

void MSegments::refresh(){
    emit layoutChanged();
}
