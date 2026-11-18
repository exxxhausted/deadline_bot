#ifndef DIALOG_STATES_HPP
#define DIALOG_STATES_HPP

#include "BotDialogState.hpp"

#include <stdexcept>

namespace lstu {

struct StartState : public BotDialogState {
    void enterState(BotDialogInput, const Update&) const override {}
    void exitState(BotDialogInput i, const Update& u) const override {
        if (i == BotDialogInput::Start)
            client->sendMessage(u.chat_id, "Привет, ленивый неудачник.");
    }
    std::string toString() const override { return "StartState"; };
};

struct EchoState : public BotDialogState {
    void enterState(BotDialogInput, const Update&) const override {}
    void exitState(BotDialogInput i, const Update& u) const override {
        if (i == BotDialogInput::Message)
            client->sendMessage(u.chat_id, u.text);
    }
    std::string toString() const override { return "EchoState"; };
};


inline
std::unique_ptr<BotDialogState>
produce(const std::string& s) {
    if (s == "StartState") return std::make_unique<StartState>();
    else if (s == "EchoState") return std::make_unique<EchoState>();
    else throw std::runtime_error("Couldn't convert string into state");
}


} // namespace lstu

#endif // DIALOG_STATES_HPP
