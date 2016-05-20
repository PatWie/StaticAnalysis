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
#ifndef MSTATICDISASSEMBLER_H
#define MSTATICDISASSEMBLER_H

#include <QtCore>
#include "../Global.h"

class MStaticDisassembler : public QThread
{
    Q_OBJECT
public:
    MStaticDisassembler();

    void setParameter(byte_t* DataPtr, quint64 DataSize, quint64 InstructionOffset, address_t BaseAddress,address_t origInstRVA);

protected:
    void run();

private:
    byte_t* m_DataPtr;
    quint64 m_DataSize;
    quint64 m_InstructionOffset;
    address_t m_BaseAddress;
    address_t m_VAOffset;

signals:
    void fullStaticDisassembly();
    void OnTargetUnloaded();
    void DisassemblyError(QString str, QString str2, QString col);
};

#endif // MSTATICDISASSEMBLER_H
