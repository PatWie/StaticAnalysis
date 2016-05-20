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
#ifndef WBREAKPOINTS_H
#define WBREAKPOINTS_H

#include <QWidget>
#include <QTreeView>
#include <QItemSelection>

#include "../Models/MBreakpoints.h"

class WBreakpoints : public QWidget
{
    Q_OBJECT

public:
    WBreakpoints(QWidget *parent=0);



public slots:

    const bool exists(address_t addr) const;
    void add(address_t addr);
    void remove(address_t addr);
    void toggle(address_t addr);

    QList<Breakpoint> breakpoints() const;
    void onDoubleClicked(QModelIndex idx);


    void refresh();
signals:
    void selectionChanged (const QItemSelection &selected);
    void gotoAddress(address_t addr);
    void updateBreakpoint(address_t addr);

private:
    void setupContent();


    QTreeView *view;
    MBreakpoints *model;

};

#endif // WBREAKPOINTS_H
