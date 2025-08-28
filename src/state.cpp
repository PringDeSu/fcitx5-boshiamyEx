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

        // reset best-choice tip first
        ic_ -> inputPanel().setPreedit(fcitx::Text());
        ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
        ic_ -> updatePreedit();

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
        if (isZhuyinMode(userInput())) {

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

    bool State::isZhuyinMode(const std::string token) const
    {
        const static std::string ZhuyinPrefix = "\';";
        return (
            token.size() >= ZhuyinPrefix.size() &&
            token.substr(0, ZhuyinPrefix.size()) == ZhuyinPrefix
        );
    }

    void State::candidateKeyEvent(fcitx::KeyEvent &event)
    {
        std::shared_ptr<fcitx::CandidateList> candidateList = ic_ -> inputPanel().candidateList();

        if (event.key().check(FcitxKey_Return) || event.key().check(FcitxKey_space)) {
            if (size()) {
                event.accept();
                if (candidateList) {
                    ic_ -> updatePreedit();
                    commitWord(candidateList, candidateList -> cursorIndex());
                }
                // std::this_thread::sleep_for(std::chrono::seconds(2));
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
                commitWord(candidateList, idx);
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
        
        const fcitx::Text preeditOld = ip.preedit();
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
        ip.setPreedit(preeditOld);

        ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
        ic_ -> updatePreedit();
    }

    void State::rerender()
    {
        ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
    }

    void State::commitWord(const std::shared_ptr<fcitx::CandidateList> candidateList, const int idx)
    {
        CandidateList * const candidateList_ = static_cast<CandidateList*>(candidateList.get());
        const CandidateWord &cand = candidateList_ -> getCandidateWord(idx);
        cand.select(ic_);
        const DictChoice dc = DictChoice(userInput(), idx);
        const std::string target = cand.getTarget();
        const DictChoice bestChoice = engine_ -> getBestChoice(target);
        if (isZhuyinMode(dc.token) || bestChoice < dc) {
            ic_ -> inputPanel().setPreedit(fcitx::Text(bestChoice.toString()));
            ic_ -> updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
            ic_ -> updatePreedit();
        }
    }

}

