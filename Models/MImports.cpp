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
#include "MImports.h"
#include "../tia.h"
#include "../Helpers/HString.h"

MImports::MImports(QObject *parent) :
    QAbstractTableModel (parent)
{

}

int MImports::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TIA::instance()->m_Target->imports().count();
}

int MImports::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}
QVariant MImports::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= TIA::instance()->m_Target->imports().count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        APIData_t el = TIA::instance()->m_Target->imports().at(index.row());
        quint64 ModulBase = 0x00400000;

        switch(index.column()){
        case ADDRESS:
            return HString::addrToString(el.apiOffset + ModulBase);
        case NAME:
            return el.apiName;
        case LIBRARY:
            return el.libraryName;
        case IDATA:
            return HString::addrToString(el.idataAddress + ModulBase);

        }


    }
    return QVariant();
}


QVariant MImports::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case NAME:
            return tr("Name");

        case ADDRESS:
            return tr("Address");

        case LIBRARY:
            return tr("Library");

        case IDATA:
            return tr(".idata Address");


        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags MImports::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}


void MImports::refresh(){
    emit layoutChanged();
}
