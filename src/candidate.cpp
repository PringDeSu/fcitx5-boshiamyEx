#include <algorithm>
#include "candidate.h"

namespace BoshiamyEx {

    /////////////////////////////// CandidateWord ///////////////////////////////

    CandidateWord::CandidateWord(const std::string target) :
        fcitx::CandidateWord(),
        target_(target)
    {
        setText(fcitx::Text(target_));
    }

    void CandidateWord::select(fcitx::InputContext *ic) const
    {
        ic -> commitString(target_);
    }

    /////////////////////////////// CandidateList ///////////////////////////////

    const std::vector<fcitx::Key> CandidateList::SELECTION_KEYS = std::vector<fcitx::Key>({
        fcitx::Key{FcitxKey_0},
        fcitx::Key{FcitxKey_1},
        fcitx::Key{FcitxKey_2},
        fcitx::Key{FcitxKey_3},
        fcitx::Key{FcitxKey_4},
        fcitx::Key{FcitxKey_5},
        fcitx::Key{FcitxKey_6},
        fcitx::Key{FcitxKey_7},
        fcitx::Key{FcitxKey_8},
        fcitx::Key{FcitxKey_9}
    });
    const int CandidateList::CANDIDATELIST_SIZE = 10;

    CandidateList::CandidateList(fcitx::InputContext *ic, const std::vector<std::string> *targets) :
        fcitx::CandidateList(),
        fcitx::PageableCandidateList(),
        fcitx::CursorMovableCandidateList(),
        fcitx::CursorModifiableCandidateList(),
        ic_(ic),
        targets_(targets),
        label_(CANDIDATELIST_SIZE),
        candidates_(CANDIDATELIST_SIZE),
        cursor_(0),
        pageStart_(0)
    {
        setPageable(this);
        setCursorMovable(this);
        setCursorModifiable(this);

        // set labels
        for (int i = 0; i < CANDIDATELIST_SIZE; i++) {
            label_[i] = fcitx::Text(std::string(1, '0' + i) + ". ");
        }

        // generating candidates data for the first time
        render();
    }

    const fcitx::Text &CandidateList::label(int idx) const
    {
        return label_[idx];
    }

    const fcitx::CandidateWord &CandidateList::candidate(int idx) const
    {
        return *candidates_[idx];
    }

    // return the current size of the candidate list
    int CandidateList::size() const
    {
        return std::min(CANDIDATELIST_SIZE, (int) (targets_ -> size() - pageStart_));
    }

    int CandidateList::cursorIndex() const
    {
        return cursor_;
    }

    fcitx::CandidateLayoutHint CandidateList::layoutHint() const
    {
        return fcitx::CandidateLayoutHint::Horizontal;
    }

    bool CandidateList::hasPrev() const
    {
        return (pageStart_ != 0);
    }

    bool CandidateList::hasNext() const
    {
        return (pageStart_ + CANDIDATELIST_SIZE < (int) (targets_ -> size()));
    }

    void CandidateList::prev()
    {
        pageStart_ -= CANDIDATELIST_SIZE;
        cursor_ = 0;
        render();
    }

    void CandidateList::next()
    {
        pageStart_ += CANDIDATELIST_SIZE;
        cursor_ = 0;
        render();
    }

    bool CandidateList::usedNextBefore() const
    {
        return false;
    }

    void CandidateList::prevCandidate()
    {
        setCursorIndex(std::max(cursor_ - 1, 0));
    }

    void CandidateList::nextCandidate()
    {
        setCursorIndex(std::min(cursor_ + 1, size() - 1));
    }

    void CandidateList::setCursorIndex(int idx)
    {
        cursor_ = idx;
    }

    void CandidateList::render()
    {
        for (int i = 0; i < CANDIDATELIST_SIZE && pageStart_ + i < (int) (targets_ -> size()); i++) {
            candidates_[i] = std::make_unique<CandidateWord>(targets_ -> at(pageStart_ + i));
        }
    }

}

