#ifndef _FCITX5_BOSHIAMYEX_CANDIDATE_H_
#define _FCITX5_BOSHIAMYEX_CANDIDATE_H_

#include <string>
#include <vector>
#include <memory>
#include <fcitx/candidatelist.h>
#include <fcitx/inputcontext.h>   // fcitx::InputContext

namespace BoshiamyEx {

    class CandidateWord :
        public fcitx::CandidateWord
    {
        public:
            CandidateWord(const std::string target);

            // pure-virtual function implementation
            void select(fcitx::InputContext *ic) const;

            // called by State functions when committing
            std::string getTarget() const;

        private:
            std::string target_;
    };

    class CandidateList :
        public fcitx::CandidateList,
        public fcitx::PageableCandidateList,
        public fcitx::CursorMovableCandidateList,
        public fcitx::CursorModifiableCandidateList
    {
        public:

            const static std::vector<fcitx::Key> SELECTION_KEYS;

            // when constructed, it is ensured that at least 1 candidate exists
            CandidateList(fcitx::InputContext *ic, const std::vector<std::string> *targets);

            // pure-virtual function implementation from fcitx::CandidateList
            const fcitx::Text &label(int idx) const;
            const fcitx::CandidateWord &candidate(int idx) const;
            int size() const;
            int cursorIndex() const;
            fcitx::CandidateLayoutHint layoutHint() const;

            // pure-virtual function implementation from fcitx::PageableCandidateList
            bool hasPrev() const;
            bool hasNext() const;
            void prev();
            void next();
            bool usedNextBefore() const;

            // pure-virtual function implementation from fcitx::CursorMovableCandidateList
            void prevCandidate();
            void nextCandidate();

            // pure-virtual function implementation from fcitx::CursorModifiableCandidateList
            void setCursorIndex(int idx);

            // used by myself (not fcitx)
            const CandidateWord &getCandidateWord(int idx) const;

        private:
            const static int CANDIDATELIST_SIZE;

            fcitx::InputContext *ic_;
            const std::vector<std::string> *targets_;

            std::vector<fcitx::Text> label_;
            std::vector<std::unique_ptr<CandidateWord>> candidates_;

            // variables for counting pages and candidates
            int cursor_;
            int pageStart_;

            // called when pageStart_ changes (reload candidates_)
            void render();
    };

}

#endif // _FCITX5_BOSHIAMYEX_CANDIDATE_H_

