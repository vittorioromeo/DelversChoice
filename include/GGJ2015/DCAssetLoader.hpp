#ifndef GGJ2015_DCASSETLOADER
#define GGJ2015_DCASSETLOADER

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
    namespace Impl
    {
        struct AssetLoader
        {
            ssvs::AssetManager<> assetManager;

            inline AssetLoader()
            {
                ssvs::loadAssetsFromJson(
                    assetManager, "Data/", ssvj::fromFile("Data/assets.json"));
            }
        };
    }
}

#endif
