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
#include "WBreakpoints.h"
#include "../Models/MBreakpoints.h"
#include "../Helpers/HString.h"
#include <QGridLayout>
#include <QDebug>
#include <QTreeView>

WBreakpoints::WBreakpoints(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    view = new QTreeView ;
    view->setIndentation(0);
    //content->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(view,0,0);

    connect(view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleClicked(QModelIndex)));

    setupContent();
}

void WBreakpoints::toggle(address_t addr){
    if(exists(addr)){
        remove(addr);
    }else{
        add(addr);
    }
    emit updateBreakpoint(addr);
}

QList<Breakpoint> WBreakpoints::breakpoints() const
{
    return model->breakpoints();
}

const bool WBreakpoints::exists(address_t addr) const
{
    return model->exists(addr);
}

void WBreakpoints::add(address_t addr)
{
    if(!exists(addr)){
        model->insertRows(0, 1, QModelIndex());
        QModelIndex index;

        index = model->index(0, MBreakpoints::Entries::OFFSET, QModelIndex());
        model->setData(index, addr, Qt::EditRole);

        index = model->index(0, MBreakpoints::Entries::SIZE, QModelIndex());
        model->setData(index, 1, Qt::EditRole);

        index = model->index(0, MBreakpoints::Entries::TYPE, QModelIndex());
        model->setData(index, BP_SOFTWARE, Qt::EditRole);

        index = model->index(0, MBreakpoints::Entries::ACTIVE, QModelIndex());
        model->setData(index, true, Qt::EditRole);


        emit updateBreakpoint(addr);
    }
}

void WBreakpoints::remove(address_t addr){
    int pos = model->row(addr);
    model->removeRow(pos,QModelIndex());
    emit updateBreakpoint(addr);
}

void WBreakpoints::setupContent()
{
    model = new MBreakpoints;
    view->setModel(model);
}


void WBreakpoints::onDoubleClicked(QModelIndex idx)
{
    emit gotoAddress(model->address(idx.row()));
}

void WBreakpoints::refresh(){
    model->refresh();
}


