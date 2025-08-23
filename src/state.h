#ifndef _FCITX5_BOSHIAMYEX_STATE_H_
#define _FCITX5_BOSHIAMYEX_STATE_H_

#include <fcitx/inputcontext.h>         // fcitx::InputContext
#include <fcitx/inputcontextproperty.h> // fcitx::InputContextProperty
#include <fcitx-utils/inputbuffer.h>    // fcitx::InputBuffer

namespace BoshiamyEx {

    class Engine; // method declaration in engine.h

    class State :
        public fcitx::InputContextProperty,
        public fcitx::InputBuffer
    {
        public:
            State(Engine *engine, fcitx::InputContext *ic);

            // called by Engine::keyEvent
            void keyEvent(fcitx::KeyEvent &event);

            // called by Engine::reset
            void reset();

        private:
            Engine *engine_;
            fcitx::InputContext *ic_;
            
            // called by keyEvent
            bool isZhuyinMode() const;

            // called by keyEvent
            void candidateKeyEvent(fcitx::KeyEvent &event);

            // construct CandidateList and draw UI
            void updateUI();

            // only update CandidateList
            void rerender();
    };

}

#endif // _FCITX5_BOSHIAMYEX_STATE_H_

