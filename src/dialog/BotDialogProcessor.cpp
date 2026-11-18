#include "BotDialogProcessor.hpp"
#include "dialog_states.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <regex>

using namespace lstu;

static const std::regex command("^/[a-zA-Z]+");

BotDialogInput BotDialogProcessor::parse(const Update& update) {
    const auto& text = update.text;

    std::smatch m;
    if (std::regex_search(text, m, command)) {
        auto command = m.str();

        if (command == "/start") return BotDialogInput::Start;
        else if (command == "/reset") return BotDialogInput::Reset;
    }

    return BotDialogInput::Message;
}

BotDialogProcessor::BotDialogProcessor(const std::string& filename) :
    m_filename(filename)
{
    using json = nlohmann::json;

    std::ifstream file(filename);
    if(!file) throw std::runtime_error("file \"" + filename + "\" not found");

    json j;
    file >> j;
    std::unordered_map<std::int64_t, std::unique_ptr<BotDialogState>> states;
    for (auto it = j.begin(); it != j.end(); ++it) {
        auto chatId = static_cast<std::int64_t>(std::stoll(it.key()));
        auto state  = produce(it.value().get<std::string>());
        states.emplace(chatId, std::move(state));
    }
    m_currentStateMap = std::move(states);
    m_transitionMap.insert({
        {{"StartState", BotDialogInput::Start},   "EchoState"},
        {{"StartState", BotDialogInput::Message}, "StartState"},
        {{"StartState", BotDialogInput::Reset},   "StartState"},
        {{"EchoState",  BotDialogInput::Start},   "StartState"},
        {{"EchoState",  BotDialogInput::Message}, "EchoState"},
        {{"EchoState",  BotDialogInput::Reset},   "StartState"},
    });
}

BotDialogProcessor::~BotDialogProcessor() {
    using json = nlohmann::json;

    std::ofstream file(m_filename);
    if (!file) return;

    json j = json::object();
    for (const auto& [chatId, state] : m_currentStateMap)
        j[std::to_string(chatId)] = state->toString();

    file << j.dump(2);
}

void BotDialogProcessor::process(const std::vector<Update>& updates) {
    for(const auto& update : updates) {
        if (update.text.empty()) continue;

        if (m_currentStateMap.count(update.chat_id) == 0)
            m_currentStateMap[update.chat_id] = produce(m_initialState);

        auto inputSymbol = parse(update);
        const auto& destination = m_transitionMap.at({m_currentStateMap[update.chat_id]->toString(), inputSymbol});

        m_currentStateMap[update.chat_id]->exitState(inputSymbol, update);
        m_currentStateMap[update.chat_id] = produce(destination);
        m_currentStateMap[update.chat_id]->enterState(inputSymbol, update);
    }
}


