#include "enginefactory.h"
#include "engine.h"         // BoshiamyEx::Engine

namespace BoshiamyEx {

    fcitx::AddonInstance *EngineFactory::create(fcitx::AddonManager *manager)
    {
        return new Engine(manager -> instance());
    }

}

FCITX_ADDON_FACTORY(BoshiamyEx::EngineFactory);

