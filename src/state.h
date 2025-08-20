#ifndef _FCITX5_BOSHIAMYEX_STATE_H_
#define _FCITX5_BOSHIAMYEX_STATE_H_

#include <fcitx/inputcontext.h>         // fcitx::InputContext
#include <fcitx/inputcontextproperty.h> // fcitx::InputContextProperty

namespace BoshiamyEx {

    class Engine; // method declaration in engine.h

    class State :
        public fcitx::InputContextProperty
    {
        public:
            State(Engine *engine, fcitx::InputContext *ic);

        private:
            Engine *engine_;
            fcitx::InputContext *ic_;
    };

}

#endif // _FCITX5_BOSHIAMYEX_STATE_H_

