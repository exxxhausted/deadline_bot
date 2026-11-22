#ifndef BOTDIALOGPROCESSOR_HPP
#define BOTDIALOGPROCESSOR_HPP

#include <functional>
#include <unordered_map>
#include <memory>

#include "IClient.hpp"
#include "Task.hpp"
#include "TaskRepository.hpp"

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

    using Handler = std::function<DialogState(const StateMachineInput&)>;

    std::unordered_map<DialogState, Handler> m_transitionHandlers;
    const DialogState m_initialState = DialogState::Start;

    std::shared_ptr<IClient> m_client;
    TaskRepository m_repo;

    std::unordered_map<std::int64_t, DialogState> m_sessions;
    std::unordered_map<std::int64_t, TaskDraft> m_drafts;

public:
    BotDialogProcessor(std::shared_ptr<IClient> client,
                       const std::string& repoPath);

    void stateMachineStep(const Update& update);

    void restoreSession(std::int64_t chatId);
    void saveSessions();

private:

    static StateMachineInput parseUpdate(const Update&);
};

} // namespace lstu

#endif // BOTDIALOGPROCESSOR_HPP
