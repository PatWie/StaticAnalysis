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
#ifndef MFUNCTIONS_H
#define MFUNCTIONS_H

#include <QAbstractTableModel>
#include <QVariant>

#include "../Global.h"
#include "../Models/MTarget.h"



class MFunctions : public QAbstractTableModel
{
    Q_OBJECT
    QMap<address_t, APIData_t > m_ApiCalls;    // <address, description>
    QMap<address_t, address_t > m_Functions;   // <start,end>

   enum Entries {NAME,ADDRESS};

public:
    MFunctions(QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;



    void addApiCall(address_t addr, APIData_t dat);
    const APIData_t getApiCall(address_t addr) const;
    QMap<address_t, APIData_t> *getApiCalls();
    const bool isApiCall(address_t target) const;


    void addFunction(address_t start_addr,address_t ret_addr);

    void clear();
    const address_t functionStart(int idx);

    void refresh();
private:


};

#endif // MFUNCTIONS_H
