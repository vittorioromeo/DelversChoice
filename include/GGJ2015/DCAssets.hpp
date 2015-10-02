#ifndef GGJ2015_DCASSETS
#define GGJ2015_DCASSETS

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCAssetLoader.hpp"

#define CACHE_ASSET_IMPL(mType, mName, mExt) \
    mType* mName{                            \
    &assetLoader.assetManager.get<mType>(VRM_PP_TOSTR(mName) mExt)};
#define CACHE_ASSETS_FOR_IMPL(mIdx, mData, mArg) \
    CACHE_ASSET_IMPL(VRM_PP_TPL_ELEM(mData, 0), mArg, VRM_PP_TPL_ELEM(mData, 1))
#define CACHE_ASSETS(mType, mExt, ...) \
    VRM_PP_FOREACH_REVERSE(            \
    CACHE_ASSETS_FOR_IMPL, VRM_PP_TPL_MAKE(mType, mExt), __VA_ARGS__)

namespace ggj
{
namespace Impl
{
    struct Assets
    {
        AssetLoader assetLoader;

        // Audio players
        ssvs::SoundPlayer soundPlayer;
        ssvs::MusicPlayer musicPlayer;

        // BitmapFonts
        CACHE_ASSETS(ssvs::BitmapFont, "", fontObStroked, fontObBig)

        // Textures
        CACHE_ASSETS(sf::Texture, ".png", slotChoice, slotChoiceBlocked,
        iconHPS, iconATK, iconDEF, drops, enemy, blocked, back, dropsModal,
        advance, itemCard, eFire, eWater, eEarth, eLightning, eST, eWK, eTY,
        equipCard, wpnMace, wpnSword, wpnSpear, armDrop, panelsmall, panellog)

        // Sounds
        CACHE_ASSETS(sf::SoundBuffer, ".ogg", lvl1, lvl2, lvl3, lvl4, menu,
        powerup, drop, grab, equipArmor, equipWpn, lose)

        std::vector<sf::SoundBuffer *> swordSnds, maceSnds, spearSnds;

        inline Assets()
        {
            for(const auto& e :
            {"normal"s, "fire"s, "water"s, "earth"s, "lightning"s})
            {
                swordSnds.emplace_back(
                &assetLoader.assetManager.get<sf::SoundBuffer>(
                "sword/" + e + ".ogg"));
                maceSnds.emplace_back(
                &assetLoader.assetManager.get<sf::SoundBuffer>(
                "mace/" + e + ".ogg"));
                spearSnds.emplace_back(
                &assetLoader.assetManager.get<sf::SoundBuffer>(
                "spear/" + e + ".ogg"));
            }

            soundPlayer.setVolume(100.f);
        }
    };
}

inline auto& getAssets() noexcept
{
    static Impl::Assets result;
    return result;
}

inline auto mkTxtOBSmall()
{
    ssvs::BitmapText result{*getAssets().fontObStroked};
    result.setTracking(-3);
    return result;
}
inline auto mkTxtOBBig()
{
    ssvs::BitmapText result{*getAssets().fontObBig};
    result.setTracking(-1);
    return result;
}
}

#endif
