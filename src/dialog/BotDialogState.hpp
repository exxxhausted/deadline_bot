#ifndef BOTDIALOGSTATE_HPP
#define BOTDIALOGSTATE_HPP

#include <memory>
#include <string>

#include "BotDialogInput.hpp"
#include "TelegramClient.hpp"

namespace lstu {

struct BotDialogState {
    static std::shared_ptr<TelegramClient> client;
    virtual ~BotDialogState() = default;
    virtual void enterState(BotDialogInput, const Update&) const = 0;
    virtual void exitState(BotDialogInput, const Update&) const = 0;
    virtual std::string toString() const = 0;
};

} // namespace lstu

#endif // BOTDIALOGSTATE_HPP
