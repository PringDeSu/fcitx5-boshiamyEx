#include "engine.h"
#include "state.h"  // we should know that BoshiamyEx::State inherits from fcitx::InputContextProperty

namespace BoshiamyEx {

    Engine::Engine(fcitx::Instance *instance) :
        dict(),
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

        // only filtered out key-releasing event and shortcut
        if (keyEvent.isRelease() || keyEvent.key().states()) {
            return;
        }

        // call State::keyEvent()
        keyEvent.inputContext() -> propertyFor(&factory_) -> keyEvent(keyEvent);
    }

    void Engine::reset(const fcitx::InputMethodEntry &entry, fcitx::InputContextEvent &event)
    {
        FCITX_UNUSED(entry);
        // call State::reset()
        event.inputContext() -> propertyFor(&factory_) -> reset();
    }

}

