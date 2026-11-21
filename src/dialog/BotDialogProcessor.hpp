#ifndef BOTDIALOGPROCESSOR_HPP
#define BOTDIALOGPROCESSOR_HPP

#include <functional>
#include <unordered_map>
#include <memory>

#include "IClient.hpp"

namespace lstu {

class BotDialogProcessor {

    enum class State {
        Start = 0,
        Echo
    };

    struct StateMachineInput {
        enum class Kind {
            Command,
            Text
        };

        std::int64_t chatId;
        Kind kind;
        std::string_view text;
    };

    using Handler = std::function<State(const StateMachineInput&)>;

    std::unordered_map<State, Handler> m_transitionHandlers;
    const State m_initialState = State::Start;
    std::shared_ptr<IClient> m_client;
    std::unordered_map<std::int64_t, State> m_sessions;

public:

    explicit BotDialogProcessor(std::shared_ptr<IClient> client);

    void stateMachineStep(const Update& update);

private:

    static StateMachineInput parseUpdate(const Update&);
};

} // namespace lstu

#endif // BOTDIALOGPROCESSOR_HPP
