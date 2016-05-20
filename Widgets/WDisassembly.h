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
#ifndef WDisassembly_H
#define WDisassembly_H

#include <QWidget>
#include <QSignalMapper>
#include "../Views/VCode.h"
#include "../Views/VJump.h"

class WDisassembly : public QWidget
{
    Q_OBJECT

    VCode* m_Code;
    VJump* m_Jump;
    QSignalMapper *signalMapper;

public:
    explicit WDisassembly(QWidget *parent = 0);




signals:
    void onDisassemblyViewReady();

    void forceRepaint();
public slots:
    void insertContent();
    void onEditLabel();
    void onEditComment();
    void onConvertDigit(NUMBERFORMAT nf);

    void Test();
    void onToBin();
    void onToOct();
    void onToDec();
    void onToHex();
    void onToAscii();
    void onGotoAddress(address_t addr);
    void onForceRepaint();
protected slots:
    void onGotoDlg();
    void onRenameVariable();
};

#endif // WDisassembly_H
