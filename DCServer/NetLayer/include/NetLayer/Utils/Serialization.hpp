#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

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

            inline Pckt() {}

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

// Defines getters and setters for a packet field.
/*#define NL_DEFINE_PCKT_PROXY(mI, mName) \
    inline const auto& mName() const& { return std::get<mI>(this->fields); } \
    inline auto& mName() & { return std::get<mI>(this->fields); }             \
    inline auto&& mName() && { return std::move(std::get<mI>(this->fields)); }
*/
#define NL_DEFINE_PCKT_PROXY(mI, mName)                                     \
    inline const auto& mName() const { return std::get<mI>(this->fields); } \
    inline auto& mName() { return std::get<mI>(this->fields); }


// Implementation of the preprocessor loop that generates the type list for
// packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL(mIdx, mData, mArg) \
    VRM_PP_TPL_EXPLODE(VRM_PP_TPL_ELEM(mArg, 0)) VRM_PP_COMMA_IF(mIdx)

// Preprocessor loop that generates the type list for packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(...) \
    VRM_PP_FOREACH_REVERSE(                    \
        NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL, ~, __VA_ARGS__)



// Implementation of the preprocessor loop that generates the fields inside the
// packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(mIdx, mData, mArg) \
    NL_DEFINE_PCKT_PROXY(mIdx, VRM_PP_TPL_ELEM(mArg, 1))

// Preprocessor loop that generates the fields inside the packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST(...) \
    VRM_PP_FOREACH(NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL, ~, __VA_ARGS__)



#define NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)                         \
    struct mName : ::nl::Impl::Pckt<NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST( \
                       VRM_PP_TPL_EXPLODE(mFieldTpls))>                \
    {                                                                  \
        NL_IMPL_DEFINE_PCKT_BODY_LIST(VRM_PP_TPL_EXPLODE(mFieldTpls))  \
    }

#define NL_DEFINE_PCKT(mName, mFieldTpls) NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)

#define NL_DEFINE_PCKT_1(mName, mTpl)                                    \
    struct mName                                                         \
        : ::nl::Impl::Pckt<VRM_PP_TPL_ELEM(VRM_PP_TPL_ELEM(mTpl, 0), 0)> \
    {                                                                    \
        NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(0, ~, mTpl)               \
    }

/*
struct AuthRequest : nl::Pckt
<
    int,                // 0
    std::string,        // 1
    float,              // 2
    std::vector<int>    // 3
>
{
    NL_DEFINE_PCKT_PROXY(0, requestID);
    NL_DEFINE_PCKT_PROXY(1, requestUser);
    NL_DEFINE_PCKT_PROXY(2, requestPriority);
    NL_DEFINE_PCKT_PROXY(3, secondaryIDs);
};
*/



template <typename T>
inline auto& operator<<(nl::PcktBuf& mP, const std::vector<T>& mX)
{
    mP << ssvu::toNum<nl::UInt32>(mX.size());
    for(const auto& x : mX) mP << x;

    return mP;
}
template <typename T>
inline auto& operator>>(nl::PcktBuf& mP, std::vector<T>& mX)
{
    // Get size
    nl::UInt32 sz;
    mP >> sz;

    mX.clear();
    mX.resize(sz);

    for(decltype(sz) i(0); i < sz; ++i) mP >> mX[i];

    return mP;
}

template <typename... Ts>
inline auto& operator<<(nl::PcktBuf& mP, const ssvu::Tpl<Ts...>& mX)
{
    ssvu::tplFor(
        [&mP, &mX](const auto& mV)
        {
            mP << mV;
        },
        mX);
    return mP;
}
template <typename... Ts>
inline auto& operator>>(nl::PcktBuf& mP, ssvu::Tpl<Ts...>& mX)
{
    ssvu::tplFor(
        [&mP, &mX](auto& mV)
        {
            mP >> mV;
        },
        mX);
    return mP;
}

template <typename... Ts>
inline auto& operator<<(nl::PcktBuf& mP, const nl::Impl::Pckt<Ts...>& mX)
{
    return mP << mX.fields;
}
template <typename... Ts>
inline auto& operator>>(nl::PcktBuf& mP, nl::Impl::Pckt<Ts...>& mX)
{
    return mP >> mX.fields;
}

template <typename T>
void serialize_nl(nl::PcktBuf& p, T&& x)
{
    p << FWD(x);
}

template <typename T>
void deserialize_nl(nl::PcktBuf& p, T& x)
{
    p >> x;
}
