#include "dict.h"
#include <fcitx-utils/standardpaths.h>  // std::filesystem, fcitx::StandardPaths
#include <fcitx-utils/log.h>            // FCITX_INFO()
#include <fstream>
#include <algorithm>

namespace BoshiamyEx {

    DictChoice::DictChoice() :
        token(39, ' '),
        pos(0)
    {}

    DictChoice::DictChoice(const std::string _token, const int _pos) :
        token(_token),
        pos(_pos)
    {}
        
    bool DictChoice::operator<(const DictChoice &other) const {
        return (
            token.size() == other.token.size() ?
            pos < other.pos :
            token.size() < other.token.size()
        );
    }

    Dict::Dict() :
        dict_(),
        bestChoice_()
    {
        // get the dict file name
        const std::filesystem::path DICT_TXT_FILE_DIR =
            fcitx::StandardPaths::global().fcitxPath("pkgdatadir") / "boshiamyEx";

        const std::filesystem::path DICT_TXT_FILE_NAME[] = { "additional.dict", "boshiamy.dict" };

        for (auto &file_name : DICT_TXT_FILE_NAME) {

            std::filesystem::path completePath = DICT_TXT_FILE_DIR / file_name;
            FCITX_INFO() << "Loading boshiamyEx dictionary: " << completePath << " ...";
            std::ifstream dict_file(completePath);

            // constructing token-target pairs
            if (dict_file.is_open()) {

                std::string target, token;
                while (dict_file >> token >> target) {

                    auto &v = dict_[token];
                    int pos = v.size();
                    v.push_back(target);
                    bestChoice_[target] = std::min(bestChoice_[target], DictChoice(token, pos));

                }

                dict_file.close();
                FCITX_INFO() << "Imported " << completePath << ".";

            } else {

                FCITX_INFO() << "Failed to open " << completePath << ". Skipped";

            }
        }
    }
    
    std::vector<std::string> Dict::getCandidateVector(const std::string token) const
    {
        auto it = dict_.find(token);
        return (it == dict_.end() ? std::vector<std::string>() : it -> second);
    }

    std::string Dict::getCandidate(const std::string token, const int pos) const
    {
        auto it = dict_.find(token);
        return (
            it == dict_.end() ? "" :
            pos >= (int) (it -> second.size()) ? "" :
            it -> second.at(pos)
        );
    }

    DictChoice Dict::getBestChoice(const std::string target) const
    {
        auto it = bestChoice_.find(target);
        return (it == bestChoice_.end() ? DictChoice() : it -> second);
    }
}

