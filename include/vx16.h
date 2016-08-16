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

    typedef uint8_t byte_t;
    typedef uint16_t word_t;

    struct Address
    {
        word_t m_segment;
        word_t m_offset;
    };

    class Memory
    {
    public:
        template <typename T>
        T get(word_t segment, word_t offset) const
        {
            const Page& page = m_storage[segment];
            return *reinterpret_cast<const T*>(&page[offset]);
        }

        template <typename T>
        T get(Address address) const
        {
            return get<T>(address.m_segment, address.m_offset);
        }

        template <typename T>
        void set(word_t segment, word_t offset, T value)
        {
            Page& page = m_storage[segment];
            *reinterpret_cast<T*>(&page[offset]) = value;
        }

        template <typename T>
        void set(Address address, T value)
        {
            return set(address.m_segment, address.m_offset, value);
        }

        word_t allocPage()
        {
            m_storage.emplace_back();
            return pageCount() - 1;
        }

        word_t pageCount() const
        {
            return static_cast<word_t>(m_storage.size());
        }

    private:
        static const size_t PAGE_SIZE = 64 * 1024;

        typedef std::array<byte_t, PAGE_SIZE> Page;
        typedef std::vector<Page> Storage;

        Storage m_storage;
    };

#define DEFINE_REGISTER_CLASS(SIZE)            \
    class Register##SIZE                       \
    {                                          \
        friend class CPU;                      \
                                               \
        constexpr Register##SIZE(byte_t index) \
        : m_index(index)                       \
        {                                      \
        }                                      \
                                               \
        byte_t m_index;                        \
    }

    DEFINE_REGISTER_CLASS(8);
    DEFINE_REGISTER_CLASS(16);

#undef DEFINE_REGISTER_CLASS

    class CPU
    {
    public:
        explicit CPU(Memory* memory)
        : m_memory(memory)
        , m_ax(0), m_bx(0), m_cx(0), m_dx(0)
        , m_bp(0), m_si(0), m_di(0), m_sp(0)
        , m_ds(m_memory->allocPage())
        , m_ss(m_memory->allocPage())
        , m_es(0), m_fs(0), m_gs(0)
        , m_flags(2)
        {
        }

        Memory* memory() const { return m_memory; }

        Address address(word_t offset) const
        {
            Address result = { ds(), offset };
            return std::move(result);
        }

        Address address(Register16 reg, word_t offset) const
        {
            Address result = { m_registers16[reg.m_index], offset };
            return std::move(result);
        }

        byte_t al() const { return m_al; }
        byte_t ah() const { return m_ah; }
        byte_t bl() const { return m_bl; }
        byte_t bh() const { return m_bh; }
        byte_t cl() const { return m_cl; }
        byte_t ch() const { return m_ch; }
        byte_t dl() const { return m_dl; }
        byte_t dh() const { return m_dh; }

        word_t ax() const { return m_ax; }
        word_t bx() const { return m_bx; }
        word_t cx() const { return m_cx; }
        word_t dx() const { return m_dx; }

        word_t bp() const { return m_bp; }
        word_t si() const { return m_si; }
        word_t di() const { return m_di; }
        word_t sp() const { return m_sp; }

        word_t ds() const { return m_ds; }
        word_t ss() const { return m_ss; }
        word_t es() const { return m_es; }
        word_t fs() const { return m_fs; }
        word_t gs() const { return m_gs; }

        word_t flags() const { return m_flags; }

        static constexpr Register8 AL{ 0 };
        static constexpr Register8 AH{ 1 };
        static constexpr Register8 BL{ 2 };
        static constexpr Register8 BH{ 3 };
        static constexpr Register8 CL{ 4 };
        static constexpr Register8 CH{ 5 };
        static constexpr Register8 DL{ 6 };
        static constexpr Register8 DH{ 7 };

        static constexpr Register16 AX{ 0 };
        static constexpr Register16 BX{ 1 };
        static constexpr Register16 CX{ 2 };
        static constexpr Register16 DX{ 3 };

        static constexpr Register16 BP{ 4 };
        static constexpr Register16 SI{ 5 };
        static constexpr Register16 DI{ 6 };
        static constexpr Register16 SP{ 7 };

        static constexpr Register16 DS{  8 };
        static constexpr Register16 SS{  9 };
        static constexpr Register16 ES{ 10 };
        static constexpr Register16 FS{ 11 };
        static constexpr Register16 GS{ 12 };

        static constexpr Register16 FLAGS{ 13 };

        void mov(Register8 reg, byte_t imm)
        {
            m_registers8[reg.m_index] = imm;
        }

        void mov(Register8 regDst, Register8 regSrc)
        {
            m_registers8[regDst.m_index] = m_registers8[regSrc.m_index];
        }

        void mov(Register16 reg, word_t imm)
        {
            m_registers16[reg.m_index] = imm;
        }

        void mov(Register16 regDst, Register16 regSrc)
        {
            m_registers16[regDst.m_index] = m_registers16[regSrc.m_index];
        }

        template <typename T>
        void mov(Address address, T imm)
        {
            m_memory->set(address, imm);
        }

        void mov(Address address, Register8 reg)
        {
            m_memory->set(address, m_registers8[reg.m_index]);
        }

        void mov(Address address, Register16 reg)
        {
            m_memory->set(address, m_registers16[reg.m_index]);
        }

        void mov(Register8 reg, Address address)
        {
            m_registers8[reg.m_index] = m_memory->get<byte_t>(address);
        }

        void mov(Register16 reg, Address address)
        {
            m_registers16[reg.m_index] = m_memory->get<word_t>(address);
        }

    private:
        Memory* m_memory;

        static const size_t REGISTER_COUNT = 14;

        union
        {
            struct
            {
#define DEFINE_REGISTER(NAME)   \
    union                       \
    {                           \
        word_t m_##NAME##x;     \
        struct                  \
        {                       \
            byte_t m_##NAME##l; \
            byte_t m_##NAME##h; \
        };                      \
    }

                DEFINE_REGISTER(a);
                DEFINE_REGISTER(b);
                DEFINE_REGISTER(c);
                DEFINE_REGISTER(d);

#undef DEFINE_REGISTER

                word_t m_bp;
                word_t m_si;
                word_t m_di;
                word_t m_sp;

                word_t m_ds;
                word_t m_ss;
                word_t m_es;
                word_t m_fs;
                word_t m_gs;

                word_t m_flags;
            };

            byte_t m_registers8 [REGISTER_COUNT * 2];
            word_t m_registers16[REGISTER_COUNT    ];
        };
    };

} // namespace vx16

#endif // !VX16_H_INCLUDED
