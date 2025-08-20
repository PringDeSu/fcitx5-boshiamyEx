#include "engine.h"
#include "state.h"  // we should know that BoshiamyEx::State inherits from fcitx::InputContextProperty

namespace BoshiamyEx {

    Engine::Engine(fcitx::Instance *instance) :
        instance_(instance),
        factory_([this](fcitx::InputContext &ic) -> State* {
            return new State(this, &ic);
        })
    {
        // to be continued: load char map
        // useful resourses:
        // #include <fcitx-utils/standardpaths.h>
        // fcitx::StandardPaths::global().fcitxPath("pkgdatadir")
    }

    void Engine::keyEvent(const fcitx::InputMethodEntry &entry, fcitx::KeyEvent &keyEvent)
    {
        FCITX_UNUSED(entry);
        FCITX_INFO() << keyEvent.key() << " isRelease=" << keyEvent.isRelease();
    }

    void Engine::reset(const fcitx::InputMethodEntry &entry, fcitx::InputContextEvent &event)
    {
        // to be continued
        FCITX_UNUSED(entry);
        FCITX_UNUSED(event);
    }

}

