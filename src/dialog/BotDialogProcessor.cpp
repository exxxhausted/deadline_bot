#include "BotDialogProcessor.hpp"

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
    m_client(client)
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

void BotDialogProcessor::stateMachineStep(const StateMachineInput& r) {
    if(m_sessions.count(r.chatId) == 0)
        m_sessions.emplace(r.chatId, m_initialState);
    auto& currentState = m_sessions.at(r.chatId);
    currentState = (m_transitionHandlers.at(currentState))(r);
}

void BotDialogProcessor::process(const Update& update) {
    auto input = parseUpdate(update);
    stateMachineStep(input);
}

void BotDialogProcessor::process(const std::vector<Update>& updates) {
    for(const auto& update : updates)
        process(update);
}


