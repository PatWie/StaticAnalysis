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

#include "MStaticDisassembler.h"
#include "../Models/MTarget.h"

#include "../tia.h"

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

#include <QDebug>

MStaticDisassembler::MStaticDisassembler()
{
}

void MStaticDisassembler::setParameter(byte_t* DataPtr, quint64 DataSize, quint64 InstructionOffset, address_t BaseAddress, address_t origInstRVA)
{
    m_DataPtr = DataPtr;
    m_DataSize = DataSize;
    m_InstructionOffset = InstructionOffset;
    m_BaseAddress = BaseAddress;
    m_VAOffset = origInstRVA;
}

void MStaticDisassembler::run()
{

    // (byte_t *DataPtr, quint64 DataSize, quint64 InstructionOffset, address_t BaseAddress, address_t VAOffset)
    /*
     * DataPtr  points to the memory of bytes
     * DataSize hold the size of the pointed data
     * InstructionOffset helps to not start in the first byte of the DataPtr
     * BaseAddress is usually 0x401000
     * VAOffset is usually    0x000000
     *
     *
     * this function should perform a static analyse of the given memory range
     */

    MTarget* target = TIA::instance()->m_Target;

    int InstrLength;
    DISASM BeaEngineStruct;

    // Reset Disasm Structure
    memset(&BeaEngineStruct, 0, sizeof(DISASM));

#ifdef _AMD64_
    BeaEngineStruct.Archi = 64;
#else
    BeaEngineStruct.Archi = 0;
#endif

    BeaEngineStruct.EIP = (UIntPtr)((quint64)m_DataPtr + (address_t)m_InstructionOffset);
    BeaEngineStruct.VirtualAddr = m_BaseAddress + m_VAOffset;

    /*  qDebug() << "full disasm starts at " << QString("%1").arg(BeaEngineStruct.VirtualAddr,8,16,QChar('0'));
        qDebug() << "full disasm ends   at " << QString("%1").arg(BeaEngineStruct.VirtualAddr+m_DataSize,8,16,QChar('0'));
        qDebug() << "data size             " << m_DataSize;
        qDebug() << "minstr offset         " << m_InstructionOffset;
        qDebug() << BeaEngineStruct.VirtualAddr;
        qDebug() << m_BaseAddress + m_VAOffset+m_DataSize;
    */
    while(BeaEngineStruct.VirtualAddr <  m_BaseAddress + m_VAOffset + m_DataSize){

        BeaEngineStruct.SecurityBlock = (UIntPtr)( (quint64) m_DataSize - m_InstructionOffset + m_BaseAddress);
        InstrLength = Disasm(&BeaEngineStruct);

        if (InstrLength == OUT_OF_BLOCK) {
            emit DisassemblyError("disassembly error: ","not allowed to read more memory","#FF0000");
            return;
        }
        else if (InstrLength == UNKNOWN_OPCODE) {
            emit DisassemblyError("disassembly error:","unknown opcode","#FF0000");
            return;
        }
        else {
            target->insertInstruction(BeaEngineStruct.VirtualAddr,BeaEngineStruct);

            m_InstructionOffset += InstrLength;
            BeaEngineStruct.EIP += InstrLength;
            BeaEngineStruct.VirtualAddr += InstrLength;
        }
    }
    emit fullStaticDisassembly();

}
