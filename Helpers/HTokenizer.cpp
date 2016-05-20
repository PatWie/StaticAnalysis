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
#include "HTokenizer.h"
#include "HString.h"

#include "../tia.h"

/*
struct _Disasm {
   UIntPtr EIP;
   UInt64 VirtualAddr;                      // current virtual address
   UIntPtr SecurityBlock;
   char CompleteInstr[INSTRUCT_LENGTH];     // we won't use this (it is the instruction as a string)
   UInt32 Archi;                            // 32bit, 64bit ,...
   UInt64 Options;
   INSTRTYPE Instruction;                   // stores the instruction
   ARGTYPE Argument1;                       // each instructions can have up to 3 arguments
   ARGTYPE Argument2;
   ARGTYPE Argument3;
   PREFIXINFO Prefix;
   UInt32 Reserved_[40];
};
*/

void HTokenizer::addToken(QList<Token_t> *Tok,const ELS type, const QString text, const quint64 info){
    Token_t ans;
    ans.Type = type;
    ans.Text = text.trimmed();  // string without whitespace
    ans.Info = info;
    Tok->append(ans);
}


void HTokenizer::Address(QList<Token_t> *Tok, const DISASM *Instr){
    // address of instruction is given in the BeaEngine struct
    addToken(Tok,T_ADDRESS,HString::addrToString(Instr->VirtualAddr),Instr->VirtualAddr);
}

void HTokenizer::Label(QList<Token_t> *Tok,const DISASM *Instr){
    // label is in the current database with key "current address"
    if( TIA::instance()->wLabels->exists(Instr->VirtualAddr)  ){
        addToken(Tok,T_LABEL,TIA::instance()->wLabels->get(Instr->VirtualAddr),Instr->VirtualAddr);
    }

}

void HTokenizer::Instruction(QList<Token_t> *Tok,const DISASM *Instr){
    // get the mnemonic from the instruction
    addToken(Tok,T_INSTRUCTION,QString(Instr->Instruction.Mnemonic));
}

void HTokenizer::Prefix(QList<Token_t> *Tok,const DISASM *Instr){
    if(Instr->Prefix.Number){
        QStringList Mne = QString("%1").arg(Instr->CompleteInstr).split(" ");
        addToken(Tok,T_PREFIX,Mne[0]);

    }

}


void HTokenizer::Argument(QList<Token_t> *Tok,const DISASM *Instr, const ARGTYPE Argument){

    // just for the instruction
    QStringList m_SegmentNames; m_SegmentNames << "" <<"es" << "ds" << "fs" << "gs" << "cs" << "ss";

    // lookup table for printer of asm code
    QMap<int,QString> m_SizeNames;
    m_SizeNames.insert(8,"byte");
    m_SizeNames.insert(16,"word");
    m_SizeNames.insert(32,"dword");
    m_SizeNames.insert(64,"qword");
    m_SizeNames.insert(80,"tword");
    m_SizeNames.insert(128,"dqword");
    m_SizeNames.insert(256,"yword");
    m_SizeNames.insert(512,"zword");



    //
    QString ArgMne = QString(Argument.ArgMnemonic).trimmed();

    if(Argument.ArgType!=NO_ARGUMENT && ArgMne != "")
    {
        if(Argument.ArgType&MEMORY_TYPE)
        {
            // #size ptr #segment:[#BaseRegister + #IndexRegister*#Scale + #Displacement]
            // example: mov eax, dword ptr ss:[ESI + EAX * 8 + 0xff]
            //                   +---------------------------------+
            Token_t segment, size, ptr, doubledot, bra1, bra2;

            // find size (dword,...)
            size.Type = T_SIZE;
            size.Text = m_SizeNames.find(Argument.ArgSize).value();
            size.Info = Argument.SegmentReg;
            Tok->append(size);

            ptr.Type = T_TEXT;
            ptr.Text = " ptr ";
            Tok->append(ptr);

            // add the abbreviation of the segment (like "ds" for data segment)
            segment.Type = T_SEGMENT;
            segment.Text = m_SegmentNames.at(Argument.SegmentReg);
            segment.Info = Argument.SegmentReg;
            Tok->append(segment);

            doubledot.Type = T_CHAR;
            doubledot.Text = ":";
            Tok->append(doubledot);

            bra1.Type = T_CHAR;
            bra1.Text = "[";
            Tok->append(bra1);

            // try to figure out how many parts the argument have
            /* possibilities
             * #BaseRegister
             * #BaseRegister + #Displacement
             * #BaseRegister + #IndexRegister*#Scale + #Displacement
             */

            int parts = 0;
            if(Argument.Memory.BaseRegister != NULL)
                parts++;
            if(Argument.Memory.IndexRegister != NULL)
                parts++;
            if(Argument.Memory.Displacement != 0){
                parts++;
            }
            parts--;

            // extract register
            if(Argument.Memory.BaseRegister != NULL){
                Token_t basereg;
                basereg.Type = T_REGISTER;
                basereg.Info = Argument.Memory.BaseRegister;

                // try to extract registername (use the mnemonic)
                QString Reg = "";
                for(int i=0;i<ArgMne.length();i++){
                    // is next character a letter?
                    if(ArgMne[i].isLetter())
                        Reg.append(ArgMne[i]);
                    else
                        break;
                }
                basereg.Text = Reg;
                Tok->append(basereg);
            }

            // "+" ?
            if(parts>0){
                Token_t plus;
                plus.Text = "+";
                plus.Type = T_CHAR;
                Tok->append(plus);
                parts--;
            }

            // Indexregister
            // TODO!!!!!!!!!!!!!!

            // Displacement
            quint64 GivenAdress=Argument.Memory.Displacement;
            if(Argument.ArgType&RELATIVE_)
                GivenAdress=Instr->Instruction.AddrValue;

            if(GivenAdress != 0){

                if(TIA::instance()->wLabels->exists(GivenAdress)){
                    // can be a label or a variable

                    // target is in the data segment ? --> it is a variable
                    if(TIA::instance()->m_Target->SegmentDataStart() <= GivenAdress){
                        // "var_?????"
                        Token_t tmp;
                        tmp.Type = T_VARIABLE;
                        tmp.Text = TIA::instance()->wLabels->get(GivenAdress);
                        tmp.Info = GivenAdress;
                        Tok->append(tmp);
                    }else{
                        // target is in the code segment --> it is a jump location
                        // "loc_?????"
                        Token_t tmp;
                        tmp.Type = T_LABEL;
                        tmp.Text = TIA::instance()->wLabels->get(GivenAdress);
                        tmp.Info = GivenAdress;
                        Tok->append(tmp);
                    }


                }else{
                    // is constant
                    Token_t tmp;
                    tmp.Type = T_CONSTANT;
                    // get the number format (hex,dec,oct,bin) from the database
                    NUMBERFORMAT NF = TIA::instance()->m_DB->numberFormat(Instr->VirtualAddr,GivenAdress);
                    tmp.Text = HString::inFormat(GivenAdress,NF);
                    tmp.Info = GivenAdress;
                    Tok->append(tmp);

                }

            }

            // close the bracket
            bra2.Type = T_CHAR;
            bra2.Text = "]";
            Tok->append(bra2);


        }
        else
        {
            // argument ist constant
            quint64 GivenAdress=Instr->Instruction.Immediat;
            if(!GivenAdress)
                GivenAdress=Instr->Instruction.AddrValue;

            if(HString::isHex(ArgMne)){

                // can be label
                if(TIA::instance()->wLabels->exists(GivenAdress)){
                    Token_t tmp;
                    tmp.Text = TIA::instance()->wLabels->get(GivenAdress);
                    tmp.Info = GivenAdress;

                    if(TIA::instance()->wFunctions->isApiCall(GivenAdress)){
                        tmp.Type = T_IMPORT;
                    }
                    else if(TIA::instance()->m_Target->SegmentDataStart() <= GivenAdress){
                        // "var_?????"
                        tmp.Type = T_VARIABLE;
                    }else{
                        // "loc_?????"
                        tmp.Type = T_LABEL;
                    }
                    Tok->append(tmp);

                }else{
                    // is number
                    Token_t tmp;
                    tmp.Type = T_CONSTANT;

                    // format ?
                    NUMBERFORMAT NF = TIA::instance()->m_DB->numberFormat(Instr->VirtualAddr,GivenAdress);

                    tmp.Text = HString::inFormat(GivenAdress,NF);
                    tmp.Info = GivenAdress;

                    Tok->append(tmp);
                }

            }else{
                // is register
                Token_t basereg;
                basereg.Type = T_REGISTER;
                basereg.Info = Argument.Memory.BaseRegister;

                // try to extract registername
                QString Reg = "";
                for(int i=0;i<ArgMne.length();i++){
                    if(ArgMne[i].isLetter())
                        Reg.append(ArgMne[i]);
                    else
                        break;
                }
                basereg.Text = Reg;
                Tok->append(basereg);
            }

        }
    }


}


void HTokenizer::Comment(QList<Token_t> *Tok, const DISASM *Instr)
{
    // do we have a comment in our database with key (current virutal address)?
    if(TIA::instance()->wComments->exists(Instr->VirtualAddr))
    {
        Token_t c;
        c.Text = TIA::instance()->wComments->get(Instr->VirtualAddr);
        c.Type = T_COMMENT;
        Tok->append(c);
    }
}

void HTokenizer::Tokenize(const DISASM *Instr, QList<Token_t> *Tok)
{
    // clear current list
    Tok->clear();

    //--  Address(Tok,Instr);
    //--  Label(Tok,Instr);

    Prefix(Tok,Instr);

    // get basic instruction like "mov, push, stosb"
    Instruction(Tok,Instr);

    // get arguments in temporary token-lists to place the comma between them if we need one
    QList<Token_t> Arg1, Arg2, Arg3;
    Argument(&Arg1,Instr,Instr->Argument1);
    Argument(&Arg2,Instr,Instr->Argument2);
    Argument(&Arg3,Instr,Instr->Argument3);

    // append the first argument
    *Tok << Arg1;

    // do we need a comma (makes sure, that tere is no comma between push and constant like
    // "push , 10h"
    if(Arg2.count()>0 && Arg1.count() >0){
        Token_t tmp;
        tmp.Text = ", ";
        tmp.Type = T_CHAR;
        Tok->append(tmp);
    }

    // append second argument (can be empty)
    *Tok << Arg2;

    // do we need a comma ?
    if(Arg3.count()>0){
        Token_t tmp;
        tmp.Text = ", ";
        tmp.Type = T_CHAR;
        Tok->append(tmp);
    }

    // append third argument (can be empty)
    *Tok << Arg3;

}
