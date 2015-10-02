#pragma once
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

namespace nl
{
namespace Impl
{
    template <typename... TFields>
    struct Pckt
    {
        ssvu::Tpl<TFields...> fields;
    };
}
}

// Defines getters and setters for a packet field.
#define NL_DEFINE_PCKT_PROXY(mI, mName)                                       \
    inline const auto& mName() const & { return std::get<mI>(this->fields); } \
    inline auto& mName() & { return std::get<mI>(this->fields); }             \
    inline auto mName() && { return std::move(std::get<mI>(this->fields)); }

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
                   VRM_PP_TPL_EXPLODE(mFieldTpls))>                    \
    {                                                                  \
        NL_IMPL_DEFINE_PCKT_BODY_LIST(VRM_PP_TPL_EXPLODE(mFieldTpls))  \
    }

#define NL_DEFINE_PCKT(mName, mFieldTpls) NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)

#define NL_DEFINE_PCKT_1(mName, mTpl)                                \
    struct mName                                                     \
    : ::nl::Impl::Pckt<VRM_PP_TPL_ELEM(VRM_PP_TPL_ELEM(mTpl, 0), 0)> \
    {                                                                \
        NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(0, ~, mTpl)           \
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
NL_DEFINE_PCKT(AuthRequest,
(((int), requestID), ((std::string), requestUser), ((float), requestPriority),
               ((std::map<int, float>), secondaryIDs)));

namespace MPL = ::ecs::MPL;

namespace experiment
{
// Settings forward-declaration.
template <typename>
struct Settings;

template <typename TIDType = std::size_t>
struct Settings
{
    using IDType = TIDType;
};

template <typename TType>
struct PcktBind
{
    using Type = TType;
};

template <typename T>
using PcktBindType = typename T::Type;

template <typename... Ts>
using PcktBinds = MPL::TypeList<Ts...>;

template <typename TList>
using PcktBindsTypes = MPL::Map<PcktBindType, TList>;

template <typename TSettings, typename TPcktBinds>
struct Config
{
    using Settings = TSettings;
    using PcktBinds = TPcktBinds;
    using BindsTypes = PcktBindsTypes<PcktBinds>;

    // static_assert validity of settings
    // static_assert validity of packet binds

    template <typename T>
    static constexpr auto hasPcktBindFor() noexcept
    {
        return MPL::Contains<T, BindsTypes>{};
    }

    template <typename T>
    static constexpr auto getPcktBindID() noexcept
    {
        return MPL::IndexOf<T, BindsTypes>{};
    }

    template <typename T>
    using PcktTypes = MPL::TypeList<int, float, char>; // TODO
};

template <typename TConfig, typename T>
auto toPayload(const T& mX)
{
    static_assert(TConfig::template hasPcktBindFor<T>(), "");
    constexpr auto id(TConfig::template getPcktBindID<T>());

    nl::Impl::Payload p;
    p << id;

    using PcktTypes = typename TConfig::template PcktTypes<T>;
    MPL::forTypes<PcktTypes>([&p, &mX](auto t)
    {
        p << std::get<ECS_TYPE(t)>(mX.fields);
    });

    return p;
}

// TODO: dispatch

template <typename TConfig, typename T>
T fromPayload(nl::Impl::Payload& mP)
{
    // TODO: assumes id has been read
    // typename TConfig::Settings::PcktIDType id;
    // mP >> id;

    T result;

    using PcktTypes = typename TConfig::template PcktTypes<T>;
    MPL::forTypes<PcktTypes>([&mP, &result](auto t)
    {
        mP >> std::get<ECS_TYPE(t)>(result.fields);
    });
}

// structure(...) and (...)destructure methods
// payload -> packet
// packet -> payload
}


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
