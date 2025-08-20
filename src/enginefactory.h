#ifndef _FCITX5_BOSHIAMYEX_ENGINEFACTORY_H_
#define _FCITX5_BOSHIAMYEX_ENGINEFACTORY_H_

#include <fcitx/addonfactory.h>   // fcitx::AddonFactory, fcitx::AddonInstance
#include <fcitx/addonmanager.h>   // fcitx::AddonManager

namespace BoshiamyEx {
    
    class EngineFactory :
        public fcitx::AddonFactory
    {
        public:
            // constructed when this addon is selected for the first time since fcitx starts
            // this is triggered at the first time we switch the IM to BoshiamyEx
            fcitx::AddonInstance *create(fcitx::AddonManager *manager) override;
    };


}

#endif // _FCITX5_BOSHIAMYEX_ENGINEFACTORY_H_

