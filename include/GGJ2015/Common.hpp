#ifndef GGJ2015_COMMON
#define GGJ2015_COMMON

#include <SSVStart/SSVStart.hpp>

namespace ggj
{
    using ssvu::SizeT;
    using ssvu::FT;
    using ssvs::Vec2;
    using ssvs::UPtr;
    using ssvs::Vec2i;
    using ssvs::Vec2f;
    using ssvs::Vec2u;
    using ssvs::Input::Trigger;


    // TODO: test, cleanup, to ssvu, remove in gen?
    template <typename TW = float>
    class WeightedChance
    {
    private:
        TW weightTotal{0.f};
        std::vector<TW> weights;

    public:
        inline void reserve(ssvu::SizeT mX) { weights.reserve(mX); }

        inline void add(TW mWeight)
        {
            weights.emplace_back(mWeight);
            weightTotal += mWeight;
        }

        inline auto get() const noexcept
        {
            auto r(ssvu::getRndR(0.f, weightTotal));

            for(auto i(0u); i < weights.size(); ++i)
            {
                if(r < weights[i]) return i;
                r -= weights[i];
            }

            SSVU_UNREACHABLE();
        }
    };

    template <typename TW = float, typename... TArgs>
    inline auto mkWeightedChance(TArgs&&... mArgs)
    {
        WeightedChance<TW> result;
        result.reserve(sizeof...(TArgs));
        ssvu::forArgs(
            [&result](auto&& mX)
            {
                result.add(FWD(mX));
            },
            FWD(mArgs)...);
        return result;
    }

    template <typename TW = float, typename... TFns>
    class WeightedSwitch
    {
    private:
        WeightedChance<TW> weightedSwitch;
        ssvu::Tpl<TFns...> fns;

    public:
    };

    /*template<typename TW = float, typename... TArgs> inline auto
    makeWeightedSwitch(TArgs&&... mArgs)
    {
        WeightedSwitch<TW> result;
        result.reserve(sizeof...(TArgs));
        ssvu::forArgs([&result](auto&& mX){ result.add(FWD(mX)); },
    FWD(mArgs)...);
        return result;
    }*/
}



#endif
