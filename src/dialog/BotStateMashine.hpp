#ifndef BOTSTATEMASHINE_HPP
#define BOTSTATEMASHINE_HPP

#include <map>
#include <functional>

#include "BotDialogInput.hpp"
#include "BotDialogState.hpp"
#include "TelegramClient.hpp"

namespace lstu {

class BotStateMashine {
    using TransitionMap = std::map<std::pair<BotDialogState, BotDialogInput>,
                                   std::function<BotDialogState(BotDialogInput, const Update&)>>;

    TransitionMap transitions_;

    // Нужен репозиторий.
public:

};

} // namespace lstu

#endif // BOTSTATEMASHINE_HPP
