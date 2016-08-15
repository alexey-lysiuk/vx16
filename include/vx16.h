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

#ifndef VX16_H_INCLUDED
#define VX16_H_INCLUDED

#include <cstdint>
#include <array>
#include <vector>

namespace vx16
{

    class Memory
    {
    public:
        template <typename T>
        T get(const uint16_t segment, const uint16_t offset) const
        {
            Page& page = m_storage[segment];
            return *reinterpret_cast<const T*>(&page[offset]);
        }

        uint16_t allocPage()
        {
            m_storage.emplace_back();
            return pageCount() - 1;
        }

        uint16_t pageCount() const
        {
            return static_cast<uint16_t>(m_storage.size());
        }

    private:
        static const size_t PAGE_SIZE = 64 * 1024;

        typedef std::array<uint8_t, PAGE_SIZE> Page;
        typedef std::vector<Page> Storage;

        Storage m_storage;
    };

    class CPU
    {
    public:
        explicit CPU(Memory& memory)
        : m_memory(memory)
        {
            m_ds = m_memory.allocPage();
            m_ss = m_memory.allocPage();
        }

        CPU(const CPU& other)
        : m_memory(other.m_memory)
        {
        }

        CPU(CPU&& other)
        : m_memory(std::move(other.m_memory))
        {
        }

        CPU& operator=(const CPU&) = delete;
        CPU& operator=(CPU&&) = delete;

        uint8_t al() const { return m_al; }
        uint8_t ah() const { return m_ah; }
        uint8_t bl() const { return m_bl; }
        uint8_t bh() const { return m_bh; }
        uint8_t cl() const { return m_cl; }
        uint8_t ch() const { return m_ch; }
        uint8_t dl() const { return m_dl; }
        uint8_t dh() const { return m_dh; }

        uint16_t ax() const { return m_ax; }
        uint16_t bx() const { return m_bx; }
        uint16_t cx() const { return m_cx; }
        uint16_t dx() const { return m_dx; }

        uint16_t bp() const { return m_bp; }
        uint16_t si() const { return m_si; }
        uint16_t di() const { return m_di; }
        uint16_t sp() const { return m_sp; }

        uint16_t ds() const { return m_ds; }
        uint16_t ss() const { return m_ss; }
        uint16_t es() const { return m_es; }
        uint16_t fs() const { return m_fs; }
        uint16_t gs() const { return m_gs; }

        uint16_t flags() const { return m_flags; }

    private:
        Memory& m_memory;

#define DEFINE_REGISTER(NAME)     \
    union                         \
    {                             \
        uint16_t m_##NAME##x = 0; \
        struct                    \
        {                         \
            uint8_t m_##NAME##l;  \
            uint8_t m_##NAME##h;  \
        };                        \
    }

        DEFINE_REGISTER(a);
        DEFINE_REGISTER(b);
        DEFINE_REGISTER(c);
        DEFINE_REGISTER(d);

        uint16_t m_bp = 0;
        uint16_t m_si = 0;
        uint16_t m_di = 0;
        uint16_t m_sp = 0;

        uint16_t m_ds;
        uint16_t m_ss;
        uint16_t m_es = 0;
        uint16_t m_fs = 0;
        uint16_t m_gs = 0;

        uint16_t m_flags = 2;

#undef DEFINE_REGISTER
    };

} // namespace vx16

#endif // !VX16_H_INCLUDED
