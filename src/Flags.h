#pragma once
#include <type_traits>
#include <initializer_list>
#include <vector>

namespace vg
{
    template <typename T>
    class Flags
    {
    public:
        using TMask = typename std::underlying_type<T>::type;

    public:
        Flags(std::initializer_list<T> flagSet) :flags(0)
        {
            for (int i = 0; i < flagSet.size(); i++) flags |= (TMask) flagSet.begin()[i];
        }
        Flags() :flags(0) {}
        Flags(T flag) : flags((TMask) flag) {}
        Flags(TMask flag) : flags(flag) {}

        void Set(T flag)
        {
            flags |= (TMask) flag;
        }
        void Unset(T flag)
        {
            flags &= !(TMask) flag;
        }
        void SetState(T flag, bool state)
        {
            if (state)
                Set(flag);
            else
                Unset(flag);
        }
        bool IsSet(T flag) const
        {
            return (flags & (TMask) flag) == (TMask) flag;
        }

        Flags<T>& operator=(T flag) { flags = (TMask) flag; }

        Flags<T> operator &(T flag) { return flags & (TMask) flag; }
        Flags<T> operator ^(T flag) { return flags ^ (TMask) flag; }
        Flags<T> operator |(T flag) { return flags | (TMask) flag; }
        Flags<T> operator <<(unsigned int shift) { return flags << shift; }
        Flags<T> operator >>(unsigned int shift) { return flags >> shift; }

        Flags<T>& operator &=(T flag) { flags &= (TMask) flag; return *this; }
        Flags<T>& operator ^=(T flag) { flags ^= (TMask) flag; return *this; }
        Flags<T>& operator |=(T flag) { flags |= (TMask) flag; return *this; }
        Flags<T>& operator <<=(unsigned int shift) { flags <<= shift; return *this; }
        Flags<T>& operator >>=(unsigned int shift) { flags >>= shift; return *this; }

        operator TMask() const { return flags; }
        operator std::vector<T>() const
        {
            std::vector<T> arr;
            TMask num = flags;

            int i = 1;
            while (num != 0)
            {
                if (num & 1 == 1)
                    arr.push_back((T) i);
                num >>= 1;
                i++;
            }

            return arr;
        }

    private:
        TMask flags;
    };
}