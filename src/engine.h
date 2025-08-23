#ifndef _FCITX5_BOSHIAMYEX_ENGINE_H_
#define _FCITX5_BOSHIAMYEX_ENGINE_H_

#include <fcitx/inputmethodengine.h>  // fcitx::InputMethodEngineV2, fcitx::InputMethodEntry, 
                                      // fcitx::KeyEvent, fcix::InputContextEvent
#include <fcitx/instance.h>           // fcitx::Instance
#include <fcitx/inputcontext.h>       // fcitx::FactoryFor
#include "dict.h"

namespace BoshiamyEx {

    class State;  // declaration of methods is in state.h

    class Engine :
        public fcitx::InputMethodEngineV2,
        public Dict
    {
        public:
            // dict stores (token, targets) and (target, bestChoice) pairs, being the core of the IM
            Dict dict;

            // called by BoshiamyEx::EngineFactory::create()
            Engine(fcitx::Instance *instance);

            // triggered when a key is pressed and fcitx is in this IM
            void keyEvent(const fcitx::InputMethodEntry &entry, fcitx::KeyEvent &keyEvent) override;

            // triggered when we switch IM from this to other, or the focusing window changes
            void reset(const fcitx::InputMethodEntry &entry, fcitx::InputContextEvent &event) override;

            fcitx::Instance *getInstance() const;

        private:
            fcitx::Instance *instance_;
            fcitx::FactoryFor<BoshiamyEx::State> factory_;
    };

}

#endif // _FCITX5_BOSHIAMYEX_ENGINE_H_

