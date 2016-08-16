/*
 * vx16: Source Code Level Virtual x86 16-bit CPU
 * Copyright (C) 2016  Alexey Lysiuk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vx16.h"

namespace vx16
{

    constexpr Register8 CPU::AL;
    constexpr Register8 CPU::AH;
    constexpr Register8 CPU::BL;
    constexpr Register8 CPU::BH;
    constexpr Register8 CPU::CL;
    constexpr Register8 CPU::CH;
    constexpr Register8 CPU::DL;
    constexpr Register8 CPU::DH;

    constexpr Register16 CPU::AX;
    constexpr Register16 CPU::BX;
    constexpr Register16 CPU::CX;
    constexpr Register16 CPU::DX;

    constexpr Register16 CPU::BP;
    constexpr Register16 CPU::SI;
    constexpr Register16 CPU::DI;
    constexpr Register16 CPU::SP;

    constexpr Register16 CPU::CS;
    constexpr Register16 CPU::DS;
    constexpr Register16 CPU::SS;
    constexpr Register16 CPU::ES;
    constexpr Register16 CPU::FS;
    constexpr Register16 CPU::GS;

} // namespace vx16
