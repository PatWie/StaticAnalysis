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
#ifndef HINSTRUCTION_H
#define HINSTRUCTION_H

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

class HInstruction
{
public:
    HInstruction();

    /**
     * @brief is given instruction a jump ?
     *
     * ignores 0xFF jumps for api-calls
     *
     * @param Instr
     * @return
     */
    static bool isJump(const DISASM *Instr);
};

#endif // HINSTRUCTION_H
