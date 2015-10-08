#pragma once

#include "../Common/Common.hpp"
#include "../Pckt/PcktMacros.hpp"

namespace nl
{
    struct init_fields
    {
    };

    namespace Impl
    {
        template <typename... TFields>
        struct Pckt
        {
            using TplType = ssvu::Tpl<TFields...>;
            TplType fields;

            template <typename... Ts>
            inline Pckt(nl::init_fields, Ts&&... mX)
                : fields(FWD(mX)...)
            {
            }

            inline Pckt() = default;

            inline Pckt(const Pckt& mX) : fields(mX.fields) {}
            inline Pckt(Pckt&& mX) : fields(std::move(mX.fields)) {}

            inline Pckt& operator=(const Pckt& mX)
            {
                fields = mX.fields;
                return *this;
            }

            inline Pckt& operator=(Pckt&& mX)
            {
                fields = std::move(mX.fields);
                return *this;
            }
        };

        template <typename... TFields>
        inline bool operator==(
            const Pckt<TFields...>& a, const Pckt<TFields...>& b)
        {
            return a.fields == b.fields;
        }

        template <typename... TFields>
        inline bool operator!=(
            const Pckt<TFields...>& a, const Pckt<TFields...>& b)
        {
            return !(a == b);
        }
    }
}
