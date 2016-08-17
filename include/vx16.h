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

        FarBytePtr bytePtr(word_t offset) const
        {
            return bytePtr(DS, offset);
        }

        FarBytePtr bytePtr(NearBytePtr address) const
        {
            return bytePtr(address.m_offset);
        }

        FarBytePtr bytePtr(Register16 segment, word_t offset) const
        {
            return FarBytePtr{ m_registers16[segment.m_index], offset };
        }

        FarBytePtr bytePtr(Register16 segment, NearBytePtr address) const
        {
            return bytePtr(segment, address.m_offset);
        }
        
        FarWordPtr wordPtr(word_t offset) const
        {
            return wordPtr(DS, offset);
        }

        FarWordPtr wordPtr(NearWordPtr address) const
        {
            return wordPtr(address.m_offset);
        }

        FarWordPtr wordPtr(Register16 segment, word_t offset) const
        {
            return FarWordPtr{ m_registers16[segment.m_index], offset };
        }

        FarWordPtr wordPtr(Register16 segment, NearWordPtr address) const
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

        static constexpr Register16 CS{  8 };
        static constexpr Register16 DS{  9 };
        static constexpr Register16 SS{ 10 };
        static constexpr Register16 ES{ 11 };
        static constexpr Register16 FS{ 12 };
        static constexpr Register16 GS{ 13 };

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

        void mov(NearBytePtr address, byte_t imm)
        {
            m_memory->set(bytePtr(address), imm);
        }

        void mov(NearWordPtr address, word_t imm)
        {
            m_memory->set(wordPtr(address), imm);
        }

        void mov(NearBytePtr address, Register8 reg)
        {
            m_memory->set(bytePtr(address), m_registers8[reg.m_index]);
        }

        void mov(NearWordPtr address, Register16 reg)
        {
            m_memory->set(wordPtr(address), m_registers16[reg.m_index]);
        }

        void mov(Register8 reg, NearBytePtr address)
        {
            m_registers8[reg.m_index] = m_memory->get(bytePtr(address));
        }

        void mov(Register16 reg, NearWordPtr address)
        {
            m_registers16[reg.m_index] = m_memory->get(wordPtr(address));
        }

        void mov(FarBytePtr address, byte_t imm)
        {
            m_memory->set(address, imm);
        }

        void mov(FarWordPtr address, word_t imm)
        {
            m_memory->set(address, imm);
        }

        void mov(FarBytePtr address, Register8 reg)
        {
            m_memory->set(address, m_registers8[reg.m_index]);
        }

        void mov(FarWordPtr address, Register16 reg)
        {
            m_memory->set(address, m_registers16[reg.m_index]);
        }

        void mov(Register8 reg, FarBytePtr address)
        {
            m_registers8[reg.m_index] = m_memory->get(address);
        }

        void mov(Register16 reg, FarWordPtr address)
        {
            m_registers16[reg.m_index] = m_memory->get(address);
        }

        void push(word_t imm)
        {
            m_sp -= 2;
            m_memory->set(m_ss, m_sp, imm);
        }

        void push(Register16 reg)
        {
            word_t value = m_registers16[reg.m_index];
            push(value);
        }

        void push(NearWordPtr address)
        {
            word_t value = m_memory->get<word_t>(m_ds, address.m_offset);
            push(value);
        }

        void push(FarWordPtr address)
        {
            word_t value = m_memory->get(address);
            push(value);
        }

        word_t pop()
        {
            word_t value = m_memory->get<word_t>(m_ss, m_sp);
            m_sp += 2;
            return value;
        }

        void pop(Register16 reg)
        {
            m_registers16[reg.m_index] = pop();
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
            word_t sp = m_sp;
            push(AX);
            push(CX);
            push(DX);
            push(BX);
            push(sp);
            push(BP);
            push(SI);
            push(DI);
        }

        void popa()
        {
            pop(DI);
            pop(SI);
            pop(BP);
            m_sp += 2;
            pop(BX);
            pop(DX);
            pop(CX);
            pop(AX);
        }

        void enter(word_t size, word_t nesting)
        {
            assert(0 == nesting && "nesting is not supported yet");
            push(BP);
            m_bp = m_sp;
            m_sp -= size;
        }

        void leave()
        {
            m_sp = m_bp;
            pop(BP);
        }

    private:
        Memory* m_memory;

        static const size_t REGISTER_COUNT = 15;

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
    };

} // namespace vx16

#endif // !VX16_H_INCLUDED
