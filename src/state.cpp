#include "state.h"
#include "engine.h"

namespace BoshiamyEx {

    State::State(Engine *engine, fcitx::InputContext *ic) :
        engine_(engine),
        ic_(ic),
        // buffer_(fcitx::InputBufferOption::AsciiOnly | fcitx::InputBufferOption::FixedCursor)
        buffer_({fcitx::InputBufferOption::AsciiOnly, fcitx::InputBufferOption::FixedCursor})
    {
        buffer_.setMaxSize(39);
    }

    void State::keyEvent(fcitx::KeyEvent &event)
    {
        // keyEvent passing down to here would be a key-pressing event and have no modifier keys

        const static std::string acceptKeySyms = "abcdefghijklmnopqrstuvwxyz;\',.[]1234567890";

        if (event.key().check(FcitxKey_BackSpace)) {
            if (buffer_.size()) {
                event.accept();
                buffer_.backspace();
            }
        }

        if (event.key().check(FcitxKey_Return) || event.key().check(FcitxKey_space)) {
            if (buffer_.size()) {
                event.accept();
                commitBuffer();
                reset();
            }
        }

        if (event.key().check(FcitxKey_Escape)) {
            event.accept();
            reset();
        }

        if (event.key().isSimple() && acceptKeySyms.find(event.key().sym()) < acceptKeySyms.size()) {
            event.accept();
            buffer_.type(event.key().sym());
        }
    }

    void State::commitBuffer()
    {
        ic_ -> commitString(engine_ -> dict.getCandidate(buffer_.userInput()));
    }

    void State::reset()
    {
        buffer_.clear();
    }
}

