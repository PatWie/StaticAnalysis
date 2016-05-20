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
#ifndef MDB_H
#define MDB_H

#include <QMap>
#include <QString>
#include <QPair>
#include "../Global.h"

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

#include "../Models/MTarget.h"






class MDB
{
private:
    QMap<address_t,address_t> m_JumpTable;   // <start,end>
    QMap<address_t,int> m_JumpDeep;          // <start addr, deep>
    QMap<QPair<address_t,qint64>,NUMBERFORMAT> m_ConstantsFormats;  // < <address,value> , format>

public:

    MDB();

    const QMap<address_t, address_t >* functions() const;
    const NUMBERFORMAT numberFormat(address_t addressOfConstant, qint64 valueOfConstant) const;
    QList<address_t> jumpsHasTarget(address_t addr) const;

    void clear();
    void analyze(DISASM Instr);

    void addCall(address_t callStartAddress, address_t callTargetAddress);
    void addJump(address_t jumpStartAddress, address_t jumpTargetAddress);
    bool existsJumpFrom(address_t addr) const;
    bool isJumpTarget(address_t addr) const;


    address_t jumpTarget(address_t jumpStartaddress) const;
    const int jumpDeep(address_t jumpStartAddress) const;
    void createJumpInformation();
    QMap<address_t, APIData_t > *apiCalls();
    void addVariable(address_t variableAddress);
    void setNumberFormat(const address_t addressOfConstant, const qint64 valueOfConstant, NUMBERFORMAT numberFormat);


};

#endif // MDB_H
