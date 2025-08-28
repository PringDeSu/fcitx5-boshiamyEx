#include "dict.h"
#include <fcitx-utils/standardpaths.h>  // std::filesystem, fcitx::StandardPaths
#include <fcitx-utils/log.h>            // FCITX_INFO()
#include <fstream>
#include <algorithm>

namespace BoshiamyEx {

    DictChoice::DictChoice() :
        token(39, ' '),
        pos(-1)
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

    std::string DictChoice::toString() const {
        return token + (pos ? "(" + std::to_string(pos) + ")" : "");
    }

    Dict::Dict() :
        dict_(),
        bestChoice_()
    {
        // get the dict file name
        const std::filesystem::path DICT_FILE_DIR =
            fcitx::StandardPaths::global().fcitxPath("pkgdatadir") / "boshiamyEx";

        // const std::filesystem::path DICT_TXT_FILE_NAME[] = { "additional.dict", "boshiamy.dict" };
        const struct {
            std::filesystem::path path_;
            bool isZhuyinFile_;
        } DICT_FILE_NAME[] = {
            { "additional.dict", false },
            { "boshiamy.dict", false },
            { "zhuyin.dict", true },
        };

        for (auto file_name : DICT_FILE_NAME) {

            std::filesystem::path completePath = DICT_FILE_DIR / file_name.path_;
            FCITX_INFO() << "Loading boshiamyEx dictionary: " << completePath << " ...";
            std::ifstream dict_file(completePath);

            const static std::string ZhuyinEndingChars = " 6347";
            const static std::string ZhuyinPrefix = "\';";

            // constructing token-target pairs
            if (dict_file.is_open()) {

                std::string target, token;
                while (dict_file >> token >> target) {

                    if (file_name.isZhuyinFile_) {
                        // add space to zhuyin tokens since we can't read the space
                        token = ZhuyinPrefix + token;
                        if (ZhuyinEndingChars.find(token.back()) >= ZhuyinEndingChars.size()) {
                            token.push_back(ZhuyinEndingChars[0]);
                        }
                    }

                    auto &v = dict_[token];
                    int pos = v.size();
                    v.push_back(target);

                    // zhuyin tokens are not included in best choice
                    if (!file_name.isZhuyinFile_) {
                        bestChoice_[target] = std::min(bestChoice_[target], DictChoice(token, pos));
                    }

                }

                dict_file.close();
                FCITX_INFO() << "Imported " << completePath << ".";

            } else {

                FCITX_INFO() << "Failed to open " << completePath << ". Skipped";

            }
        }
    }
    
    const std::vector<std::string> *Dict::getCandidateVector(const std::string token) const
    {
        auto it = dict_.find(token);
        return (it == dict_.end() ? nullptr : &(it -> second));
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

