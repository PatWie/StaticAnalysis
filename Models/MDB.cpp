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
#include "MDB.h"
#include <QDebug>
#include <QList>
#include "../tia.h"

/*
 * TODO:
 * change the implementation to use real models instead of this single class --> JumpInfo
 *
 */


MDB::MDB()
{

}

/**
 * @brief clears the whole database
 */
void MDB::clear()
{ 
    m_JumpDeep.clear();
    m_JumpTable.clear();
}

/**
 * @brief changes the numberformat for displaying a constant
 * @param addressOfConstant
 * @param valueOfConstant
 * @param numberFormat
 */
void MDB::setNumberFormat(const address_t addressOfConstant, const qint64 valueOfConstant, NUMBERFORMAT numberFormat){
    m_ConstantsFormats.insert(qMakePair(addressOfConstant,valueOfConstant),numberFormat);
}

/**
 * @brief MDB::returns number format for a given constant
 *
 * multiple constants with same value in one instruction will be all changed
 *
 * @param addressOfConstant
 * @param valueOfConstant
 * @return
 */
const NUMBERFORMAT MDB::numberFormat(address_t addressOfConstant, qint64 valueOfConstant) const
{
    QMap<QPair<address_t,qint64>,NUMBERFORMAT>::const_iterator it= m_ConstantsFormats.constFind(qMakePair(addressOfConstant,valueOfConstant));
    if(it == m_ConstantsFormats.end())
        return N_HEX;
    else
        return it.value();
}

/**
 * @brief add a call information
 *
 * "0xDEAD: call 0xBEEF" should be added as "addCall(0xDEAD,0xBEEF)"
 * @param callStartAddress
 * @param callTargetAddress
 */
void MDB::addCall(address_t callStartAddress, address_t callTargetAddress){
    Q_UNUSED(callStartAddress);
    TIA::instance()->wLabels->set(callTargetAddress,"sub_"+QString("%1").arg(callTargetAddress,6,16,QChar('0')).toUpper());
    TIA::instance()->wFunctions->addFunction(callTargetAddress);
}

/**
 * @brief adds a jump information
 * @param jumpStartAddress
 * @param jumpTargetAddress
 */
void MDB::addJump(address_t jumpStartAddress, address_t jumpTargetAddress){
    TIA::instance()->wLabels->set(jumpTargetAddress,"loc_"+QString("%1").arg(jumpTargetAddress,6,16,QChar('0')).toUpper());
    m_JumpTable.insert(jumpStartAddress,jumpTargetAddress);
}

/**
 * @brief adds a variable (label for given address)
 * @param target
 */
void MDB::addVariable(address_t variableAddress){
    TIA::instance()->wLabels->set(variableAddress,"var_"+QString("%1").arg(variableAddress,6,16,QChar('0')).toUpper());

}

/**
 * @brief returns whether instruction in given address contains a jump
 * @param addr
 * @return
 */
bool MDB::existsJumpFrom(address_t addr) const{
    return m_JumpTable.contains(addr);

}

/**
 * @brief returns whether given address is target of a jump instruction
 * @param addr
 * @return
 */
bool MDB::isJumpTarget(address_t addr) const
{
    JmpIter i = m_JumpTable.begin();
    const JmpIter e = m_JumpTable.end();

    while(i != e){
        if(i.value() == addr) return true;
        i++;
    }
    return false;
}

/**
 * @brief returns all addresses that contain jumps to given address
 * @param addr
 * @return
 */
QList<address_t> MDB::jumpsHasTarget(address_t addr) const
{
    QList<address_t> Starts;

    JmpIter i = m_JumpTable.begin();
    const JmpIter e = m_JumpTable.end();

    while(i != e){
        if(i.value() == addr) Starts.append(i.key());
        i++;
    }
    return Starts;
}

/**
 * @brief returns target of jump
 * @param jumpStartaddress
 * @return
 */
address_t MDB::jumpTarget(address_t jumpStartaddress) const
{
    return m_JumpTable.constFind(jumpStartaddress).value();
}

/**
 * @brief returns heuristic deep in JumpView (left of diassembly view)
 * @param jumpStartAddress
 * @return
 */
const int MDB::jumpDeep(address_t jumpStartAddress) const
{
    return m_JumpDeep.constFind(jumpStartAddress).value();
}

/**
 * @brief heuristic to build jump information when displaying jump arrows
 */
void MDB::createJumpInformation()
{
    // reset
    for(QMap<address_t,int>::ConstIterator i=m_JumpDeep.begin();i!=m_JumpDeep.end();i++)
        m_JumpDeep[i.key()] = 0;

    for(JmpIter I=m_JumpTable.begin();I!=m_JumpTable.end();I++){

        int deep = 0;
        const address_t i_s = I.key();
        const address_t i_e = I.value();

        for(JmpIter J=m_JumpTable.begin();J!=m_JumpTable.end();J++){
            if(I.key() == J.key())
                continue;
            const address_t j_s = J.key();
            const address_t j_e = J.value();

            // if you understand this you will get a cookie
            // it is only a heuristic
            if(j_s>i_s && j_e <i_e && j_s < i_e)
                deep++;
            else if(j_s>i_s && j_s < i_e)
                deep++;
            else if(j_s>i_e && j_e<i_e && j_e>i_s)
                deep++;
            else if(j_s < i_s && j_e > i_s && j_e <i_e)
                deep++;
            //else if(j_s < i_s && j_e > i_e)
            //  deep+= m_JumpDeep[J.key()] +1;

        }
        m_JumpDeep[I.key()] = deep;

    }
}

/**
 * @brief analyzes a given instruction and updates the database
 * @param Instruction
 */
void MDB::analyze(DISASM Instruction)
{
    if(Instruction.Instruction.BranchType && Instruction.Instruction.BranchType!=RetType && !(Instruction.Argument1.ArgType &REGISTER_TYPE))
    {

        // is jump --> remember target location
        QString targetloc = Instruction.Argument1.ArgMnemonic;
        targetloc.replace("h","");

        bool isNumber;
        quint64 l = targetloc.toUInt(&isNumber,16);

        if(isNumber){
            // resolve calls ( a special kind of Jumps )
            if(Instruction.Instruction.Opcode == 0xE8){
                addCall(Instruction.VirtualAddr,l);
            }else{
                addJump(Instruction.VirtualAddr,l);

            }
        }
    }
}

