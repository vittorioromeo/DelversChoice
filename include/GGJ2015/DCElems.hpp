#ifndef GGJ2015_DCELEMS
#define GGJ2015_DCELEMS

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
inline auto createElemSprite(int mEI)
{
    static auto array(ssvu::mkArray(getAssets().eFire, getAssets().eWater,
    getAssets().eEarth, getAssets().eLightning));

    return sf::Sprite{*(array[mEI])};
}

template <typename T>
inline void appendElems(ssvs::GameWindow& mGW, const T& mX, ElementBitset mEB)
{
    for(auto i(0u); i < Constants::elementCount; ++i) {
        if(!mEB[i]) continue;

        auto offset(7 * i);
        auto s(createElemSprite(i));

        s.setPosition(mX.getPosition() + Vec2f{12.f + offset, 0.f});

        mGW.draw(s);
    }
}
}

#endif
