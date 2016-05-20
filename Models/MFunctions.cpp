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
#include "MFunctions.h"
#include "../tia.h"
#include "../Helpers/HString.h"

MFunctions::MFunctions(QObject *parent) :
    QAbstractTableModel (parent)
{

}

int MFunctions::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Functions.count();
}

int MFunctions::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}
QVariant MFunctions::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_Functions.count() || index.row() < 0)
        return QVariant();

    QMap<address_t, address_t>::const_iterator it = m_Functions.begin() + index.row();

    if(isApiCall(it.key())){
        if(role == Qt::BackgroundRole){
            return QBrush(QColor("#e0fcfc"));//#ffe1ff
        }
        if(role == Qt::FontRole && index.column() == NAME){
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
    }



    if (role == Qt::DisplayRole) {


        switch(index.column()){
        case ADDRESS:
            return HString::addrToString(it.key());
        case NAME:
            return TIA::instance()->wLabels->get(it.key());

        }


    }
    return QVariant();
}


QVariant MFunctions::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case NAME:
            return tr("Name");

        case ADDRESS:
            return tr("Address");


        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags MFunctions::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}



////////////////////////////
void MFunctions::addApiCall(address_t addr, APIData_t dat)
{
    m_ApiCalls.insert(addr,dat);
}


const APIData_t MFunctions::getApiCall(address_t addr) const
{
    return m_ApiCalls.constFind(addr).value();
}

QMap<address_t, APIData_t > *MFunctions::getApiCalls()
{
    return &m_ApiCalls;
}

const bool MFunctions::isApiCall(address_t target) const
{
    return m_ApiCalls.contains(target);
}
////////////////////////////

const address_t MFunctions::functionStart(int idx){
    return (m_Functions.begin() + idx).key();
}

void MFunctions::refresh()
{
    emit layoutChanged();
}

void MFunctions::addFunction(address_t start_addr,address_t ret_addr)
{
    m_Functions.insert(start_addr,ret_addr);
}


///////////////////////////
void MFunctions::clear()
{
    m_ApiCalls.clear();
    m_Functions.clear();
    emit layoutChanged();
}
