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

int main()
{
    Memory mem;
    testMem(mem);

    CPU cpu(mem);
    testInit(cpu);
}
