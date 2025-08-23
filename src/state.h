#ifndef _FCITX5_BOSHIAMYEX_STATE_H_
#define _FCITX5_BOSHIAMYEX_STATE_H_

#include <fcitx/inputcontext.h>         // fcitx::InputContext
#include <fcitx/inputcontextproperty.h> // fcitx::InputContextProperty
#include <fcitx-utils/inputbuffer.h>    // fcitx::InputBuffer
#include <fcitx/event.h>

namespace BoshiamyEx {

    class Engine; // declaration of methods is in engine.h

    class State :
        public fcitx::InputContextProperty
    {
        public:
            State(Engine *engine, fcitx::InputContext *ic);

            void keyEvent(fcitx::KeyEvent &event);
            void reset();

        private:
            Engine *engine_;
            fcitx::InputContext *ic_;
            fcitx::InputBuffer buffer_;

            void commitBuffer();
    };

}

#endif // _FCITX5_BOSHIAMYEX_STATE_H_

