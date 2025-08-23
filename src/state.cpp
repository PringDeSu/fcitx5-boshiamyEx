#include "state.h"
#include "engine.h"
#include <fcitx-utils/log.h>

namespace BoshiamyEx {

    State::State(Engine *engine, fcitx::InputContext *ic) :
        fcitx::InputContextProperty(),
        fcitx::InputBuffer({ fcitx::InputBufferOption::AsciiOnly, fcitx::InputBufferOption::FixedCursor }),
        engine_(engine),
        ic_(ic)
    {
        setMaxSize(39);
    }

    void State::keyEvent(fcitx::KeyEvent &event)
    {
        // events passing down are key-pressing events without modifier keys

        const static std::string NormalAcceptKeySyms = "abcdefghijklmnopqrstuvwxyz;\',.[]";
        const static std::string ZhuyinAcceptKeySyms = "1qaz2wsxedcrfv5tgbyhnujm8ik,9ol.0p;/- 6347";
        const static std::string ZhuyinEndingKeySyms = " 6347";

        if (event.key().check(FcitxKey_BackSpace)) {
            // backspace
            if (size()) { // without "if", we end up with no functionality of backspace
                event.accept();
                backspace();
            }
            return;
        }

        if (event.key().check(FcitxKey_Escape)) {
            // reset
            event.accept();
            reset();
            return;
        }

        // append input to buffer or call candidateKeyEvent
        // here criteria depends on whether we are in Zhuyin mode
        if (isZhuyinMode()) {

            if (ZhuyinEndingKeySyms.find(userInput().back()) < ZhuyinEndingKeySyms.size()) {
                // assuming the token is completely entered
                candidateKeyEvent(event);
                return;
            }

            // append to the buffer if char accepted
            if (
                event.key().isSimple() &&
                ZhuyinAcceptKeySyms.find(event.key().sym()) < ZhuyinAcceptKeySyms.size()
            ) {
                event.accept();
                type(event.key().sym());
                return;
            }

            // once append fails, pass to candidateKeyEvent
            candidateKeyEvent(event);
            return;

        }

        // here we are at Normal mode
        if (
            event.key().isSimple() &&
            NormalAcceptKeySyms.find(event.key().sym()) < NormalAcceptKeySyms.size()
        ) {
            event.accept();
            type(event.key().sym());
            return;
        }

        // once append fails, pass to candidateKeyEvent
        candidateKeyEvent(event);
    }

    void State::reset()
    {
        clear();
    }

    bool State::isZhuyinMode() const
    {
        const static std::string ZhuyinPrefix = "\';";
        return (
            size() >= ZhuyinPrefix.size() &&
            userInput().substr(0, ZhuyinPrefix.size()) == ZhuyinPrefix
        );
    }

    void State::candidateKeyEvent(fcitx::KeyEvent &event)
    {
        if (event.key().check(FcitxKey_Return) || event.key().check(FcitxKey_space)) {
            // commit string
            event.accept();
            ic_ -> commitString(engine_ -> getCandidate(userInput()));
            reset();
        }
    }

}

