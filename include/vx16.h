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
        struct Register8
        {
            uint8_t m_index;
        };

        struct Register16
        {
            uint8_t m_index;
        };

    public:
        explicit CPU(Memory& memory)
        : m_memory(memory)
        , m_ax(0), m_bx(0), m_cx(0), m_dx(0)
        , m_bp(0), m_si(0), m_di(0), m_sp(0)
        , m_ds(m_memory.allocPage())
        , m_ss(m_memory.allocPage())
        , m_es(0), m_fs(0), m_gs(0)
        , m_flags(2)
        {
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

        static constexpr Register8 AL = { 0 };
        static constexpr Register8 AH = { 1 };
        static constexpr Register8 BL = { 2 };
        static constexpr Register8 BH = { 3 };
        static constexpr Register8 CL = { 4 };
        static constexpr Register8 CH = { 5 };
        static constexpr Register8 DL = { 6 };
        static constexpr Register8 DH = { 7 };

        static constexpr Register16 AX = { 0 };
        static constexpr Register16 BX = { 1 };
        static constexpr Register16 CX = { 2 };
        static constexpr Register16 DX = { 3 };

        static constexpr Register16 BP = { 4 };
        static constexpr Register16 SI = { 5 };
        static constexpr Register16 DI = { 6 };
        static constexpr Register16 SP = { 7 };

        static constexpr Register16 DS = {  8 };
        static constexpr Register16 SS = {  9 };
        static constexpr Register16 ES = { 10 };
        static constexpr Register16 FS = { 11 };
        static constexpr Register16 GS = { 12 };

        static constexpr Register16 FLAGS = { 13 };

    private:
        Memory& m_memory;

        static const size_t REGISTER_COUNT = 14;

        union
        {
            struct
            {
#define DEFINE_REGISTER(NAME)    \
    union                        \
    {                            \
        uint16_t m_##NAME##x;    \
        struct                   \
        {                        \
            uint8_t m_##NAME##l; \
            uint8_t m_##NAME##h; \
        };                       \
    }

                DEFINE_REGISTER(a);
                DEFINE_REGISTER(b);
                DEFINE_REGISTER(c);
                DEFINE_REGISTER(d);

#undef DEFINE_REGISTER

                uint16_t m_bp;
                uint16_t m_si;
                uint16_t m_di;
                uint16_t m_sp;

                uint16_t m_ds;
                uint16_t m_ss;
                uint16_t m_es;
                uint16_t m_fs;
                uint16_t m_gs;

                uint16_t m_flags;
            };

            uint8_t  m_registers8 [REGISTER_COUNT * 2];
            uint16_t m_registers16[REGISTER_COUNT    ];
        };
    };

} // namespace vx16

#endif // !VX16_H_INCLUDED
