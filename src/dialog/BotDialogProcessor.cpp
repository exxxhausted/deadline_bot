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

BotDialogProcessor::BotDialogProcessor(std::shared_ptr<IClient> client) :
    BotDialogProcessor(client, SessionStateStore{})
{}

BotDialogProcessor::BotDialogProcessor(std::shared_ptr<IClient> client, SessionStateStore store) :
    m_client(std::move(client)),
    m_store(std::move(store))
{
    m_transitionHandlers.emplace(
        State::Start,
        [this](const StateMachineInput& r) {
            if(r.kind == StateMachineInput::Kind::Command && r.text == "/start") {
                m_client->sendMessage(r.chatId, "Greetings!");
                return State::Echo;
            } else {
                m_client->sendMessage(r.chatId, "Press /start");
                return State::Start;
            }
        }
    );

    m_transitionHandlers.emplace(
        State::Echo,
        [this](const StateMachineInput& r) {
            if(r.kind == StateMachineInput::Kind::Command && r.text == "/reset") {
                m_client->sendMessage(r.chatId, "Reseting...");
                return State::Start;
            } else {
                m_client->sendMessage(r.chatId, "Эхо: " + std::string(r.text));
                return State::Echo;
            }
        }
    );
}

void BotDialogProcessor::stateMachineStep(const Update& update) {
    auto input = parseUpdate(update);

    // TODO(этап 3): restoreSession(input.chatId);
    // Должно выполняться до обработчика: файл является источником истины.

    if(m_sessions.count(input.chatId) == 0)
        m_sessions.emplace(input.chatId, m_initialState);
    auto& currentState = m_sessions.at(input.chatId);
    currentState = (m_transitionHandlers.at(currentState))(input);

    // TODO(этап 3): saveSessions();
    // Фиксирует новое состояние сразу после перехода.
}

void BotDialogProcessor::restoreSession(std::int64_t chatId) {
    const auto saved = m_store.load(chatId);
    m_sessions[chatId] = saved.value_or(m_initialState);
}

void BotDialogProcessor::saveSessions() const {
    m_store.saveAll(m_sessions);
}


