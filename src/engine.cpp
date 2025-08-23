#include "engine.h"
#include "state.h"  // we should know that BoshiamyEx::State inherits from fcitx::InputContextProperty

namespace BoshiamyEx {

    Engine::Engine(fcitx::Instance *instance) :
        fcitx::InputMethodEngineV2(),
        Dict(),
        instance_(instance),
        factory_([this](fcitx::InputContext &ic) -> State* {
            return new State(this, &ic);
        })
    {
        instance -> inputContextManager().registerProperty("State", &factory_);
    }

    void Engine::keyEvent(const fcitx::InputMethodEntry &entry, fcitx::KeyEvent &keyEvent)
    {
        FCITX_UNUSED(entry);
        FCITX_INFO() << keyEvent.key() << " isRelease=" << keyEvent.isRelease();

        // only filter out key-releasing event and keys with modifiers
        if (keyEvent.isRelease() || keyEvent.key().states()) {
            return;
        }

        // call State::keyEvent()
        keyEvent.inputContext() -> propertyFor(&factory_) -> keyEvent(keyEvent);
    }

    void Engine::reset(const fcitx::InputMethodEntry &entry, fcitx::InputContextEvent &event)
    {
        // to be continued
        FCITX_UNUSED(entry);
        FCITX_UNUSED(event);
    }

}

