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

#include <cassert>

using namespace vx16;

void testMem(Memory& mem)
{
    assert(mem.pageCount() == 0);

    // TODO: ...
}

void testInit(CPU& cpu)
{
    assert(cpu.memory()->pageCount() >= 2);

    assert(cpu.ax() == 0);
    assert(cpu.bx() == 0);
    assert(cpu.cx() == 0);
    assert(cpu.dx() == 0);

    assert(cpu.bp() == 0);
    assert(cpu.si() == 0);
    assert(cpu.di() == 0);
    assert(cpu.sp() == 0);

    assert(cpu.ds() != cpu.ss());
    assert(cpu.es() == 0);
    assert(cpu.fs() == 0);
    assert(cpu.gs() == 0);

    assert(cpu.flags() == 2);
}

void testMovsImm(CPU& cpu)
{
    cpu.mov(CPU::AL, 16);
    assert(cpu.al() == 16);

    cpu.mov(CPU::AH, 32);
    assert(cpu.ah() == 32);
    assert(cpu.ax() == 0x2010);

    cpu.mov(CPU::BL, 111);
    assert(cpu.bl() == 111);

    cpu.mov(CPU::BH, 222);
    assert(cpu.bh() == 222);
    assert(cpu.bx() == 0xDE6F);

    cpu.mov(CPU::CL, 0xEE);
    assert(cpu.cl() == 0xEE);

    cpu.mov(CPU::CH, 0xFF);
    assert(cpu.ch() == 0xFF);
    assert(cpu.cx() == 0xFFEE);

    cpu.mov(CPU::DL, 0xCC);
    assert(cpu.dl() == 0xCC);

    cpu.mov(CPU::DH, 0xAA);
    assert(cpu.dh() == 0xAA);
    assert(cpu.dx() == 0xAACC);

    cpu.mov(CPU::AX, 0x1234);
    assert(cpu.ax() == 0x1234);

    cpu.mov(CPU::BX, 0x4321);
    assert(cpu.bx() == 0x4321);

    cpu.mov(CPU::CX, 0x1001);
    assert(cpu.cx() == 0x1001);

    cpu.mov(CPU::DX, 0xFFFF);
    assert(cpu.dx() == 0xFFFF);

    cpu.mov(CPU::BP, 0x1111);
    assert(cpu.bp() == 0x1111);

    cpu.mov(CPU::SI, 0x2727);
    assert(cpu.si() == 0x2727);

    cpu.mov(CPU::DI, 0x6789);
    assert(cpu.di() == 0x6789);

    cpu.mov(CPU::SP, 0xABCD);
    assert(cpu.sp() == 0xABCD);
}

void testMovsReg(CPU& cpu)
{
    cpu.mov(CPU::AH, CPU::AL);
    assert(cpu.al() == cpu.ah());

    cpu.mov(CPU::BH, CPU::CL);
    assert(cpu.cl() == cpu.bh());

    cpu.mov(CPU::CH, CPU::BL);
    assert(cpu.ch() == cpu.bl());

    cpu.mov(CPU::DL, CPU::AH);
    assert(cpu.ah() == cpu.dl());

    cpu.mov(CPU::DH, CPU::AL);
    assert(cpu.al() == cpu.dh());

    cpu.mov(CPU::BP, CPU::AX);
    assert(cpu.ax() == cpu.bp());

    cpu.mov(CPU::SI, CPU::BX);
    assert(cpu.bx() == cpu.si());

    cpu.mov(CPU::DI, CPU::CX);
    assert(cpu.cx() == cpu.di());

    cpu.mov(CPU::SP, CPU::DX);
    assert(cpu.dx() == cpu.sp());
}

void testMovsMem(CPU& cpu, Memory& mem)
{
    cpu.mov(cpu.wordPtr(0x10), 0x1234);
    assert(mem.get<uint16_t>(cpu.ds(), 0x10) == 0x1234);

    cpu.mov(cpu.bytePtr(CPU::DS, 0x11), 0x89);
    assert(mem.get<uint8_t>(cpu.ds(), 0x11) == 0x89);
    assert(mem.get<uint16_t>(cpu.ds(), 0x10) == 0x8934);

    cpu.mov(CPU::AX, 0xABCD);
    cpu.mov(cpu.wordPtr(0x20), CPU::AX);
    assert(mem.get<uint16_t>(cpu.ds(), 0x20) == 0xABCD);

    cpu.mov(CPU::BX, cpu.wordPtr(0x20));
    assert(cpu.bx() == 0xABCD);

    cpu.mov(CPU::ES, mem.allocPage());
    assert(cpu.ds() != cpu.es());
    assert(cpu.ss() != cpu.es());

    cpu.mov(cpu.wordPtr(CPU::ES, 0x30), 0xEFCD);
    assert(mem.get<uint16_t>(cpu.es(), 0x30) == 0xEFCD);

    cpu.mov(CPU::FS, cpu.es());
    assert(cpu.es() == cpu.fs());
    assert(mem.get<uint8_t>(cpu.fs(), 0x30) == 0xCD);
    assert(mem.get<uint8_t>(cpu.fs(), 0x31) == 0xEF);
}

int main()
{
    Memory mem;
    testMem(mem);

    CPU cpu(&mem);
    testInit(cpu);
    testMovsImm(cpu);
    testMovsReg(cpu);
    testMovsMem(cpu, mem);
}
