#pragma once

#include "../Common/Common.hpp"
#include "../Utils/CTBimap.hpp"

namespace nl
{
namespace Impl
{
    using PcktID = UInt;

    namespace PcktIDType
    {
        struct Auto
        {
        };

        template <PcktID TID>
        struct Fixed
        {
            inline static constexpr auto getID() noexcept { return TID; }
        };
    }

    template <typename T>
    struct IsPcktIDTypeAuto : ssvu::FalseT
    {
    };
    template <>
    struct IsPcktIDTypeAuto<PcktIDType::Auto> : ssvu::TrueT
    {
    };

    template <typename T>
    struct IsPcktIDTypeFixed : ssvu::FalseT
    {
    };
    template <PcktID TID>
    struct IsPcktIDTypeFixed<PcktIDType::Fixed<TID>> : ssvu::TrueT
    {
    };

    template <typename T>
    struct IsPcktIDType : ssvu::FalseT
    {
    };
    template <PcktID TID>
    struct IsPcktIDType<PcktIDType::Fixed<TID>> : ssvu::TrueT
    {
    };
    template <>
    struct IsPcktIDType<PcktIDType::Auto> : ssvu::TrueT
    {
    };

    template <typename... TFields>
    struct Pckt
    {
        ssvu::Tpl<TFields...> fields;
        inline static constexpr auto isPcktType() noexcept { return true; }
    };

    template <typename TPcktIDType, typename T>
    struct PcktType
    {
        SSVU_ASSERT_STATIC(T::isPcktType(), "T must derive from Pckt");
        SSVU_ASSERT_STATIC(
        Impl::IsPcktIDType<TPcktIDType>{}, "TPcktIDType must be a PcktIDType");

        using PcktIDType = TPcktIDType;
        using Type = T;
    };

    template <typename>
    struct IsPcktType : ssvu::FalseT
    {
    };
    template <typename... Ts>
    struct IsPcktType<PcktType<Ts...>> : ssvu::TrueT
    {
    };

    template <typename... Ts>
    struct PcktTypes
    {
        SSVU_ASSERT_STATIC_NM(ssvu::MPL::all<Impl::IsPcktType, Ts...>());
        using Type = ssvu::MPL::List<Ts...>;
    };

    template <typename T>
    struct IsPcktTypes : ssvu::FalseT
    {
    };
    template <typename... Ts>
    struct IsPcktTypes<PcktTypes<Ts...>> : ssvu::TrueT
    {
    };

    template <PcktID TPIDNext, typename...>
    struct PcktTypeIDList
    {
    };
    template <PcktID TPIDNext>
    struct PcktTypeIDList<TPIDNext>
    {
        using Type = CTBimap<>;
    };

    template <PcktID, typename>
    struct PTIL;
    template <PcktID TPIDNext>
    struct PTIL<TPIDNext, PcktIDType::Auto>
    {
        static constexpr PcktID next{TPIDNext + 1};
        static constexpr PcktID curr{TPIDNext};
    };
    template <PcktID TPIDNext, PcktID TPIDFixed>
    struct PTIL<TPIDNext, PcktIDType::Fixed<TPIDFixed>>
    {
        static constexpr PcktID next{TPIDNext};
        static constexpr PcktID curr{TPIDFixed};
    };


    template <PcktID TPIDNext, typename T, typename... Ts>
    struct PcktTypeIDList<TPIDNext, T, Ts...>
    {
        using PTILType = PTIL<TPIDNext, typename T::PcktIDType>;

        using Next = typename PcktTypeIDList<PTILType::next, Ts...>::Type;
        using Type = typename Next::template Add<
        BMPair<ssvu::CTVal<PcktID, PTILType::curr>, typename T::Type>>;

        SSVU_ASSERT_STATIC(Next::unique, "Packet IDs must be unique");
    };

    template <typename>
    struct PcktTypesExpander
    {
    };
    template <typename... Ts>
    struct PcktTypesExpander<PcktTypes<Ts...>>
    {
        using Type = PcktTypeIDList<0, Ts...>;
    };

    template <typename T>
    using PcktTypesIDListExp = typename PcktTypesExpander<T>::Type;

    template <typename TPcktTypes>
    struct PcktManager
    {
        SSVU_ASSERT_STATIC(
        Impl::IsPcktTypes<TPcktTypes>{}, "TPcktTypes must be a PcktTypes");

    private:
        using PcktTypesList = typename TPcktTypes::Type;
        using PcktTypeIDs = Impl::PcktTypesIDListExp<TPcktTypes>;
        using PcktBimap = typename PcktTypeIDs::Type;

    public:
        template <typename T>
        inline static constexpr auto getPcktID() noexcept
        {
            return Impl::LookupByValue<PcktBimap, T>{};
        }
        template <PcktID TPcktID>
        using PcktType =
        Impl::LookupByKey<PcktBimap, ssvu::CTVal<PcktID, TPcktID>>;
    };
}

template <typename T>
using PcktIDAuto = Impl::PcktType<Impl::PcktIDType::Auto, T>;

template <Impl::PcktID TID, typename T>
using PcktIDFixed = Impl::PcktType<Impl::PcktIDType::Fixed<TID>, T>;

template <typename... Ts>
using Pckt = Impl::Pckt<Ts...>;

template <typename... Ts>
using PcktSet = Impl::PcktManager<Impl::PcktTypes<Ts...>>;
}
