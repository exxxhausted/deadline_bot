#include "BotDialogProcessor.hpp"

#include <utility>

using namespace lstu;

BotDialogProcessor::StateMachineInput BotDialogProcessor::parseUpdate(const Update& u) {
    std::string_view t = u.text;
    if (t.size() > 1 && t[0] == '/') {
        auto cmd = t.substr(0, t.find_first_of(" @"));
        return {u.chatId, StateMachineInput::Kind::Command, cmd};
    }
    return {u.chatId, StateMachineInput::Kind::Text, t};
}

BotDialogProcessor::BotDialogProcessor(std::shared_ptr<IClient> client,
                                       const std::string& repoPath) :
    m_client(std::move(client)),
    m_repo(repoPath)
{
    m_transitionHandlers.emplace(
        DialogState::Start,
        [this](const StateMachineInput& input) {
            if(input.kind == StateMachineInput::Kind::Command && input.text == "/start") {
                m_client->sendMessage(input.chatId, "Greetings!");
                return DialogState::Echo;
            } else {
                m_client->sendMessage(input.chatId, "Press /start");
                return DialogState::Start;
            }
        }
    );

    m_transitionHandlers.emplace(
        DialogState::Echo,
        [this](const StateMachineInput& input) {
            if(input.kind == StateMachineInput::Kind::Command && input.text == "/reset") {
                m_client->sendMessage(input.chatId, "Reseting...");
                return DialogState::Start;
            }

            if(input.kind == StateMachineInput::Kind::Command && input.text == "/add") {
                m_drafts.insert_or_assign(input.chatId, TaskDraft(0, input.chatId));
                m_client->sendMessage(input.chatId, "Название задачи? (/cancel - отмена)");
                return DialogState::AwaitTitle;
            }

            if(input.kind == StateMachineInput::Kind::Command && input.text == "/list") {
                const auto tasks = m_repo.taskList(input.chatId);
                std::string text = tasks.empty() ? "Активных задач нет." : "Активные задачи:";
                for(int i = 0; i < tasks.size(); ++i) {
                    text += "\n" + std::to_string(i + 1) + ". " + tasks[i].title + " - до "
                            + dateToIso(tasks[i].deadline);
                }
                m_client->sendMessage(input.chatId, text);
                return DialogState::Echo;
            }

            m_client->sendMessage(input.chatId, "Эхо: " + std::string(input.text));
            return DialogState::Echo;
        }
    );

    m_transitionHandlers.emplace(
        DialogState::AwaitTitle,
        [this](const StateMachineInput& input) {
            if(input.kind == StateMachineInput::Kind::Command) {
                if(input.text == "/cancel") {
                    m_drafts.erase(input.chatId);
                    m_client->sendMessage(input.chatId, "Отменено.");
                    return DialogState::Echo;
                }
                m_client->sendMessage(input.chatId, "Жду название задачи. /cancel - отмена.");
                return DialogState::AwaitTitle;
            }

            if(input.text.find_first_not_of(" \t\r\n") == std::string_view::npos) {
                m_client->sendMessage(input.chatId, "Название не может быть пустым. Введите название:");
                return DialogState::AwaitTitle;
            }

            auto& [_, taskDraft] = *m_drafts.emplace(input.chatId, TaskDraft(0, input.chatId)).first;
            taskDraft.setTitle(std::string(input.text));
            m_client->sendMessage(input.chatId, "Дедлайн? Формат ДД-ММ-ГГГГ.");
            return DialogState::AwaitDeadline;
        }
    );

    m_transitionHandlers.emplace(
        DialogState::AwaitDeadline,
        [this](const StateMachineInput& input) {
            if(input.kind == StateMachineInput::Kind::Command) {
                if(input.text == "/cancel") {
                    m_drafts.erase(input.chatId);
                    m_client->sendMessage(input.chatId, "Отменено.");
                    return DialogState::Echo;
                }
                m_client->sendMessage(input.chatId, "Жду дедлайн в формате ДД-ММ-ГГГГ. /cancel - отмена.");
                return DialogState::AwaitDeadline;
            }

            try {
                m_drafts.at(input.chatId).setDeadline(dateFromIso(input.text));
            } catch(const std::invalid_argument&) {
                m_client->sendMessage(input.chatId, "Не понял дату. Нужен формат ДД-ММ-ГГГГ.");
                return DialogState::AwaitDeadline;
            }

            Task task = m_drafts.at(input.chatId).build();
            auto id = m_repo.addTask(task);
            m_drafts.erase(input.chatId);
            m_client->sendMessage(input.chatId, "Сохранено: <<" + task.title + ">> до "
                                                    + dateToIso(task.deadline) + ", id "
                                                    + std::to_string(id) + ".");
            return DialogState::Echo;
        }
    );
}

void BotDialogProcessor::stateMachineStep(const Update& update) {
    auto input = parseUpdate(update);

    if(m_sessions.count(input.chatId) == 0)
        restoreSession(input.chatId);

    auto& currentState = m_sessions.at(input.chatId);
    currentState = (m_transitionHandlers.at(currentState))(input);
}

void BotDialogProcessor::restoreSession(std::int64_t chatId) {
    auto saved = m_repo.getState(chatId);
    if (!saved) {
        m_sessions[chatId] = DialogState::Start;
        return;
    }
    if (*saved != DialogState::Echo)
        m_client->sendMessage(chatId, "Бот перезапускался, черновики потеряны.");

    m_sessions[chatId] = DialogState::Echo;
}

void BotDialogProcessor::saveSessions() {
    for (const auto& [chatId, state] : m_sessions)
        if (state != DialogState::Start)
            m_repo.setState(chatId, state);
}


