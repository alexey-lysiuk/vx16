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
#include <cassert>
#include <array>
#include <vector>

namespace vx16
{

    typedef uint8_t byte_t;
    typedef uint16_t word_t;

    struct NearBytePtr
    {
        word_t m_offset;
    };

    struct FarBytePtr
    {
        word_t m_segment;
        word_t m_offset;
    };

    struct NearWordPtr
    {
        word_t m_offset;
    };

    struct FarWordPtr
    {
        word_t m_segment;
        word_t m_offset;
    };

    class Memory
    {
    public:
        byte_t get(FarBytePtr address) const
        {
            return get<byte_t>(address.m_segment, address.m_offset);
        }

        void set(FarBytePtr address, byte_t value)
        {
            set(address.m_segment, address.m_offset, value);
        }

        word_t get(FarWordPtr address) const
        {
            return get<word_t>(address.m_segment, address.m_offset);
        }

        void set(FarWordPtr address, word_t value)
        {
            set(address.m_segment, address.m_offset, value);
        }

        template <typename T>
        T get(word_t segment, word_t offset) const
        {
            const Page& page = m_storage[segment];
            return *reinterpret_cast<const T*>(&page[offset]);
        }

        template <typename T>
        void set(word_t segment, word_t offset, T value)
        {
            Page& page = m_storage[segment];
            *reinterpret_cast<T*>(&page[offset]) = value;
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

    enum class R8 : byte_t
    {
        AL = 0,
        AH = 1,
        BL = 2,
        BH = 3,
        CL = 4,
        CH = 5,
        DL = 6,
        DH = 7
    };

    enum class R16 : byte_t
    {
        AX = 0,
        BX = 1,
        CX = 2,
        DX = 3,

        BP = 4,
        SI = 5,
        DI = 6,
        SP = 7,

        CS = 8,
        DS = 9,
        SS = 10,
        ES = 11,
        FS = 12,
        GS = 13,

        FLAGS = 14,

        COUNT
    };

    class CPU
    {
    public:
        explicit CPU(Memory* memory)
        : m_memory(memory)
        , m_ax(0), m_bx(0), m_cx(0), m_dx(0)
        , m_bp(0), m_si(0), m_di(0), m_sp(0)
        , m_cs(0)
        , m_ds(m_memory->allocPage())
        , m_ss(m_memory->allocPage())
        , m_es(0), m_fs(0), m_gs(0)
        , m_flags(2)
        {
        }

        Memory* memory() const { return m_memory; }

        byte_t value(R8 reg) const
        {
            const size_t index = static_cast<size_t>(reg);
            return m_registers8[index];
        }

        word_t value(R16 reg) const
        {
            const size_t index = static_cast<size_t>(reg);
            return m_registers16[index];
        }

        FarBytePtr bytePtr(word_t offset) const
        {
            return bytePtr(R16::DS, offset);
        }

        FarBytePtr bytePtr(NearBytePtr address) const
        {
            return bytePtr(address.m_offset);
        }

        FarBytePtr bytePtr(R16 segment, word_t offset) const
        {
            return FarBytePtr{ value(segment), offset };
        }

        FarBytePtr bytePtr(R16 segment, NearBytePtr address) const
        {
            return bytePtr(segment, address.m_offset);
        }
        
        FarWordPtr wordPtr(word_t offset) const
        {
            return wordPtr(R16::DS, offset);
        }

        FarWordPtr wordPtr(NearWordPtr address) const
        {
            return wordPtr(address.m_offset);
        }

        FarWordPtr wordPtr(R16 segment, word_t offset) const
        {
            return FarWordPtr{ value(segment), offset };
        }

        FarWordPtr wordPtr(R16 segment, NearWordPtr address) const
        {
            return wordPtr(segment, address.m_offset);
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

        word_t cs() const { return m_cs; }
        word_t ds() const { return m_ds; }
        word_t ss() const { return m_ss; }
        word_t es() const { return m_es; }
        word_t fs() const { return m_fs; }
        word_t gs() const { return m_gs; }

        bool cf() const { return m_cf; }
        bool pf() const { return m_pf; }
        bool af() const { return m_af; }
        bool zf() const { return m_zf; }
        bool sf() const { return m_sf; }
        bool df() const { return m_df; }
        bool of() const { return m_of; }

        word_t flags() const { return m_flags; }

        void mov(R8 reg, byte_t imm)
        {
            setValue(reg, imm);
        }

        void mov(R8 regDst, R8 regSrc)
        {
            setValue(regDst, value(regSrc));
        }

        void mov(R16 reg, word_t imm)
        {
            setValue(reg, imm);
        }

        void mov(R16 regDst, R16 regSrc)
        {
            setValue(regDst, value(regSrc));
        }

        void mov(NearBytePtr address, byte_t imm)
        {
            m_memory->set(bytePtr(address), imm);
        }

        void mov(NearWordPtr address, word_t imm)
        {
            m_memory->set(wordPtr(address), imm);
        }

        void mov(NearBytePtr address, R8 reg)
        {
            m_memory->set(bytePtr(address), value(reg));
        }

        void mov(NearWordPtr address, R16 reg)
        {
            m_memory->set(wordPtr(address), value(reg));
        }

        void mov(R8 reg, NearBytePtr address)
        {
            const byte_t value = m_memory->get(bytePtr(address));
            setValue(reg, value);
        }

        void mov(R16 reg, NearWordPtr address)
        {
            const word_t value = m_memory->get(wordPtr(address));
            setValue(reg, value);
        }

        void mov(FarBytePtr address, byte_t imm)
        {
            m_memory->set(address, imm);
        }

        void mov(FarWordPtr address, word_t imm)
        {
            m_memory->set(address, imm);
        }

        void mov(FarBytePtr address, R8 reg)
        {
            m_memory->set(address, value(reg));
        }

        void mov(FarWordPtr address, R16 reg)
        {
            m_memory->set(address, value(reg));
        }

        void mov(R8 reg, FarBytePtr address)
        {
            setValue(reg, m_memory->get(address));
        }

        void mov(R16 reg, FarWordPtr address)
        {
            setValue(reg, m_memory->get(address));
        }

        void cwd()
        {
            m_dx = (m_ax & 0x8000) ? 0xFFFF : 0;
        }

        void xlat()
        {
            m_al = m_memory->get(bytePtr(m_bx + m_al));
        }

        void push(word_t imm)
        {
            m_sp -= 2;
            m_memory->set(m_ss, m_sp, imm);
        }

        void push(R16 reg)
        {
            push(value(reg));
        }

        void push(NearWordPtr address)
        {
            const word_t value = m_memory->get<word_t>(m_ds, address.m_offset);
            push(value);
        }

        void push(FarWordPtr address)
        {
            const word_t value = m_memory->get(address);
            push(value);
        }

        word_t pop()
        {
            const word_t value = m_memory->get<word_t>(m_ss, m_sp);
            m_sp += 2;
            return value;
        }

        void pop(R16 reg)
        {
            setValue(reg, pop());
        }

        void pop(NearWordPtr address)
        {
            m_memory->set(m_ds, address.m_offset, pop());
        }

        void pop(FarWordPtr address)
        {
            m_memory->set(address, pop());
        }

        void pusha()
        {
            const word_t sp = m_sp;
            push(m_ax);
            push(m_cx);
            push(m_dx);
            push(m_bx);
            push(sp);
            push(m_bp);
            push(m_si);
            push(m_di);
        }

        void popa()
        {
            m_di = pop();
            m_si = pop();
            m_bp = pop();
            m_sp += 2;
            m_bx = pop();
            m_dx = pop();
            m_cx = pop();
            m_ax = pop();
        }

        void enter(word_t size, word_t nesting)
        {
            assert(0 == nesting && "nesting is not supported yet");
            push(m_bp);
            m_bp = m_sp;
            m_sp -= size;
        }

        void leave()
        {
            m_sp = m_bp;
            m_bp = pop();
        }

    private:
        Memory* m_memory;

        static const size_t REGISTER_COUNT = size_t(R16::COUNT);

        union
        {
            struct
            {
#define VX16_DEFINE_REGISTER(NAME) \
    union                          \
    {                              \
        word_t m_##NAME##x;        \
        struct                     \
        {                          \
            byte_t m_##NAME##l;    \
            byte_t m_##NAME##h;    \
        };                         \
    }

                VX16_DEFINE_REGISTER(a);
                VX16_DEFINE_REGISTER(b);
                VX16_DEFINE_REGISTER(c);
                VX16_DEFINE_REGISTER(d);

#undef VX16_DEFINE_REGISTER

                word_t m_bp;
                word_t m_si;
                word_t m_di;
                word_t m_sp;

                word_t m_cs;
                word_t m_ds;
                word_t m_ss;
                word_t m_es;
                word_t m_fs;
                word_t m_gs;

                union
                {
                    struct
                    {
                        word_t m_cf:1;
                        word_t m_reserved1:1;
                        word_t m_pf:1;
                        word_t m_reserved2:1;
                        word_t m_af:1;
                        word_t m_reserved3:1;
                        word_t m_zf:1;
                        word_t m_sf:1;
                        word_t m_tf:1;
                        word_t m_if:1;
                        word_t m_df:1;
                        word_t m_of:1;
                        word_t m_iopl:2;
                        word_t m_nt:1;
                        word_t m_reserved4:1;
                    };
                    word_t m_flags;
                };
            };

            byte_t m_registers8 [REGISTER_COUNT * 2];
            word_t m_registers16[REGISTER_COUNT    ];
        };

        void setValue(R8 reg, byte_t value)
        {
            const size_t index = static_cast<size_t>(reg);
            m_registers8[index] = value;
        }

        void setValue(R16 reg, word_t value)
        {
            const size_t index = static_cast<size_t>(reg);
            m_registers16[index] = value;
        }

    };

} // namespace vx16

#endif // !VX16_H_INCLUDED
