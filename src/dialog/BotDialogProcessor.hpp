#ifndef BOTDIALOGPROCESSOR_HPP
#define BOTDIALOGPROCESSOR_HPP

#include <functional>
#include <unordered_map>
#include <memory>

#include "IClient.hpp"
#include "SessionStateStorage.hpp"

namespace lstu {

class BotDialogProcessor {

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
    SessionStateStorage m_storage;

    std::unordered_map<std::int64_t, State> m_sessions;

public:

    explicit BotDialogProcessor(std::shared_ptr<IClient> client);
    BotDialogProcessor(std::shared_ptr<IClient> client, std::filesystem::path storagePath);

    void stateMachineStep(const Update& update);

    void restoreSession(std::int64_t chatId);
    void saveSessions() const;

private:

    static StateMachineInput parseUpdate(const Update&);
};

} // namespace lstu

#endif // BOTDIALOGPROCESSOR_HPP
