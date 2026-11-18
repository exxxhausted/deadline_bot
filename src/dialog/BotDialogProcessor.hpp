#ifndef BOTDIALOGPROCESSOR_HPP
#define BOTDIALOGPROCESSOR_HPP

#include <unordered_map>
#include <memory>
#include "BotDialogState.hpp"
#include "TelegramClient.hpp"

namespace lstu {

struct PairHash {
    std::size_t operator() (const std::pair<std::string, BotDialogInput>& p) const {
        return std::hash<std::string>{}(p.first);
    }
};

class BotDialogProcessor {

    using TransitionMap = std::unordered_map<std::pair<std::string, BotDialogInput>,
                                             std::string,
                                             PairHash>;

    TransitionMap m_transitionMap;

    std::unordered_map<std::int64_t, std::unique_ptr<BotDialogState>> m_currentStateMap;

    std::string m_filename;

    const std::string m_initialState = "StartState";

    BotDialogInput parse(const Update& update);

public:

    explicit BotDialogProcessor(const std::string& filename);

    ~BotDialogProcessor();

    void process(const std::vector<Update>& updates);

};

} // namespace lstu

#endif // BOTDIALOGPROCESSOR_HPP
