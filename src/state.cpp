#include "state.h"
#include "engine.h"
#include "candidate.h"
#include <fcitx-utils/log.h>
#include <fcitx/inputpanel.h>

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
            updateUI();
            return;
        }

        if (event.key().check(FcitxKey_Escape)) {
            // reset
            if (size()) { // without "if", we end up with no functionality of escape
                event.accept();
                reset();
            }
            updateUI();
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
                updateUI();
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
            updateUI();
            return;
        }

        // once append fails, pass to candidateKeyEvent
        candidateKeyEvent(event);
    }

    void State::reset()
    {
        clear();
        updateUI();
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
        std::shared_ptr<fcitx::CandidateList> candidateList = ic_ -> inputPanel().candidateList();

        if (event.key().check(FcitxKey_Return) || event.key().check(FcitxKey_space)) {
            if (size()) {
                event.accept();
                if (candidateList) {
                    candidateList -> candidate(candidateList -> cursorIndex()).select(ic_);
                }
                reset();
            }
            return;
        }
        
        // procedure below are for candidate list, so we can terminate early if there is no candidate list
        if (!candidateList) {
            return;
        }

        event.accept();

        int idx = event.key().keyListIndex(CandidateList::SELECTION_KEYS);
        if (idx >= 0) {
            // number pressed
            if (idx < candidateList -> size()) {
                candidateList -> candidate(idx).select(ic_);
                reset();
            }
            return;
        }

        if (event.key().checkKeyList(engine_ -> getInstance() -> globalConfig().defaultPrevPage())) {
            if (auto *pageable = candidateList -> toPageable(); pageable && pageable -> hasPrev()) {
                pageable -> prev();
            }
            rerender();
            return;
        }

        if (event.key().checkKeyList(engine_ -> getInstance() -> globalConfig().defaultNextPage())) {
            if (auto *pageable = candidateList -> toPageable(); pageable && pageable -> hasNext()) {
                pageable -> next();
            }
            rerender();
            return;
        }

        if (event.key().checkKeyList(engine_ -> getInstance() -> globalConfig().defaultPrevCandidate()) || event.key().check(FcitxKey_Left)) {
            if (auto *cursormovable = candidateList -> toCursorMovable(); cursormovable) {
                cursormovable -> prevCandidate();
            }
            rerender();
            return;
        }

        if (event.key().checkKeyList(engine_ -> getInstance() -> globalConfig().defaultNextCandidate()) || event.key().check(FcitxKey_Right)) {
            if (auto *cursormovable = candidateList -> toCursorMovable(); cursormovable) {
                cursormovable -> nextCandidate();
            }
            rerender();
            return;
        }

    }

    void State::updateUI()
    {
        fcitx::InputPanel &ip = ic_ -> inputPanel();
        
        ip.reset();
        
        const std::vector<std::string> *target = engine_ -> getCandidateVector(userInput());
        if (target) {
            ip.setCandidateList(std::make_unique<CandidateList>(ic_, target));
        }

        ip.setClientPreedit(fcitx::Text(
            userInput(),
            (
                ic_ -> capabilityFlags().test(fcitx::CapabilityFlag::Preedit) ?
                fcitx::TextFormatFlag::HighLight :
                fcitx::TextFormatFlag::NoFlag
            )
        ));

        ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
        ic_ -> updatePreedit();
    }

    void State::rerender()
    {
        ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
    }

}

