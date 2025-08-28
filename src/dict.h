#ifndef _FCITX5_BOSHIAMYEX_DICT_H_
#define _FCITX5_BOSHIAMYEX_DICT_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace BoshiamyEx {

    struct DictChoice
    {

        std::string token;
        int pos;

        DictChoice();
        DictChoice(const std::string _token, const int _pos);

        std::string toString() const;

        bool operator<(const DictChoice &other) const;

    };

    class Dict
    {
        public:
            Dict();
            const std::vector<std::string> *getCandidateVector(const std::string token) const;
            std::string getCandidate(const std::string token, const int pos = 0) const;
            DictChoice getBestChoice(const std::string target) const;

        private:
            // dict_ maps the input sequence (tokens) to available candidate words (targets)
            std::unordered_map<std::string, std::vector<std::string>> dict_;
            // bestChoice_ maps a word (target) to its best input sequence (token)
            std::unordered_map<std::string, DictChoice> bestChoice_;
    };

}

#endif // _FCITX5_BOSHIAMYEX_DICT_H_

