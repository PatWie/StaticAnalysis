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
#include "VImports.h"
#include "../tia.h"
#include "../Helpers/HString.h"

#include <QGridLayout>
#include <QDebug>

VImports::VImports(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    m_ImportsTable = new QTreeView ;
    layout->addWidget(m_ImportsTable,0,0);
}

void VImports::insertContent()
{
    QList<APIData_t> imports = TIA::instance()->m_Target->imports();
    qDebug() << "imports " << imports.size();
    if(imports.size() <= 0) return;

    quint64 ModulBase = 0x00400000;

    Imports = new QStandardItemModel (imports.size(),4);
    Imports->setHorizontalHeaderItem( 0, new QStandardItem( "Address" ) );
    Imports->setHorizontalHeaderItem( 1, new QStandardItem( "Name" ) );
    Imports->setHorizontalHeaderItem( 2, new QStandardItem( "Library" ) );
    Imports->setHorizontalHeaderItem( 3, new QStandardItem( "idata addr" ) );

    m_ImportsTable->setModel(Imports);

    for(int importCount = 0; importCount < imports.size(); importCount++)
    {

        QStandardItem *itm_address = new QStandardItem(HString::addrToString(imports.value(importCount).apiOffset +ModulBase));
        itm_address->setEditable(false);
        QStandardItem *itm_idata = new QStandardItem(HString::addrToString(imports.value(importCount).idataAddress +ModulBase));
        itm_address->setEditable(false);
        QStandardItem *itm_name = new QStandardItem( imports.value(importCount).apiName);
        itm_name->setEditable(false);
        QStandardItem *itm_lib = new QStandardItem( imports.value(importCount).libraryName);
        itm_lib->setEditable(false);

        Imports->setItem(importCount,0,itm_address);
        Imports->setItem(importCount,1,itm_name);
        Imports->setItem(importCount,2,itm_lib);
        Imports->setItem(importCount,3,itm_idata);


    }

    emit onImportsViewReady();
}


QSize VImports::sizeHint() const
{
    return QSize(400,100);
}
