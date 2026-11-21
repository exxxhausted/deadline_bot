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
    m_client(std::move(client)),
    m_storage()
{}

BotDialogProcessor::BotDialogProcessor(std::shared_ptr<IClient> client, std::filesystem::path storagePath) :
    m_client(std::move(client)),
    m_storage(storagePath)
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

    if(m_sessions.count(input.chatId) == 0)
        restoreSession(input.chatId);

    auto& currentState = m_sessions.at(input.chatId);
    currentState = (m_transitionHandlers.at(currentState))(input);

    // TEMPORARY SOLUTION
    saveSessions();
}

void BotDialogProcessor::restoreSession(std::int64_t chatId) {
    try {
        auto saved = m_storage.load(chatId);
        m_sessions[chatId] = saved.value_or(m_initialState);
    } catch (const StateFileNotFound&) {
        m_sessions[chatId] = m_initialState;
    }
}

void BotDialogProcessor::saveSessions() const {
    m_storage.saveAll(m_sessions);
}


