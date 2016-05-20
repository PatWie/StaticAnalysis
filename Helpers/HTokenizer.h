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
#ifndef HTOKENIZER_H
#define HTOKENIZER_H

#include <QList>
#include <QString>
#include <QStringList>

#include "../Global.h"
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"


/**
 * @brief The Token_t struct
 * a token represents a part of a disassembly instruction
 * example:
 *
 *    mov eax, 100h
 *
 *  - mov  = T_INSTRUCTION
 *  - eax  = T_REGISTER
 *  - 100h = T_CONSTANT
 *
 */
struct Token_t{
    ELS Type;
    QString Text;
    quint64 Info;
};


/**
 * @brief process instructions and creates tokens
 * contains code to analyze an instruction from the BeaEngine-Struct
 */
class HTokenizer
{
public:

    /**
     * @brief split an instruction into tokens
     *
     * returns a list of tokens from the given instruction
     *
     * @param Instr   (given instruction)
     * @param Tok     (list of tokens) , will be reseted first
     */
    static void Tokenize(const DISASM *Instr, QList<Token_t> *Tok);


private:
    /************** not in use *************************************/
    /**
     * @brief add the address token to the token list
     * @param Tok
     * @param Instr
     */
    static void Address(QList<Token_t> *Tok,const DISASM *Instr);
    /**
     * @brief add the label token to the token list
     * @param Tok
     * @param Instr
     */
    static void Label(QList<Token_t> *Tok,const DISASM *Instr);
    /**
     * @brief extract comment for given instruction
     * @param Tok
     * @param Instr
     */
    static void Comment(QList<Token_t> *Tok,const DISASM *Instr);
    /************** im  use    *************************************/
    static void Prefix(QList<Token_t> *Tok, const DISASM *Instr);
    /**
     * @brief extract basic instruction to tokenlist (mov, stosb, push, ...)
     * @param Tok
     * @param Instr
     */
    static void Instruction(QList<Token_t> *Tok,const DISASM *Instr);
    /**
     * @brief parse given argument of instruction and add it to the tokenlist
     * @param Tok
     * @param Instr
     * @param Arg
     */
    static void Argument(QList<Token_t> *Tok, const DISASM *Instr, const ARGTYPE Arg);

    /**
     * @brief helper function to add a token to a token list
     * @param Tok tokenslist
     * @param type type of token
     * @param text text to be printed for output
     * @param info additional info like jump target address
     */
    static void addToken(QList<Token_t> *Tok, const ELS type, const QString text, const quint64 info=NULL);
};

#endif // HTOKENIZER_H
