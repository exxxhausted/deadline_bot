#include "SessionStateStore.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <system_error>
#include <utility>

namespace lstu {

namespace {

constexpr int kSchemaVersion = 1;

} // namespace

using json = nlohmann::json;

SessionStateStore::SessionStateStore(std::filesystem::path file) : m_file(std::move(file)) {}

std::optional<State> SessionStateStore::load(std::int64_t chatId) const {
    std::ifstream input(m_file);
    if (!input)
        throw StateFileNotFound{m_file};

    try {
        json data;
        input >> data;

        if (!data.is_object())
            throw StateStoreError{"state file root is not an object"};

        if (data.value("version", 0) != kSchemaVersion)
            throw StateStoreError{"unsupported state file version"};

        const auto sessionsIt = data.find("sessions");
        if (sessionsIt == data.end() || !sessionsIt->is_object())
            throw StateStoreError{"state file has no sessions object"};

        const std::string key = std::to_string(chatId);
        const auto stateIt = sessionsIt->find(key);
        if (stateIt == sessionsIt->end())
            return std::nullopt;

        if (!stateIt->is_string())
            throw StateStoreError{"invalid state value for chat " + key};

        const std::string name = stateIt->get<std::string>();
        const auto state = stateFromString(name);
        if (!state)
            throw StateStoreError{"unknown state '" + name + "' for chat " + key};

        return state;
    } catch (const json::exception& error) {
        throw StateStoreError{"invalid state file " + m_file.string() + ": " + error.what()};
    }
}

void SessionStateStore::saveAll(const std::unordered_map<std::int64_t, State>& sessions) const {
    if (m_file.has_parent_path()) {
        std::error_code error;
        std::filesystem::create_directories(m_file.parent_path(), error);
        if (error)
            throw StateStoreError{"cannot create state file directory: " + error.message()};
    }

    json serializedSessions = json::object();
    for (const auto& [chatId, state] : sessions)
        serializedSessions[std::to_string(chatId)] = std::string{toString(state)};

    json data;
    data["version"] = kSchemaVersion;
    data["sessions"] = std::move(serializedSessions);

    const std::filesystem::path temp = m_file.string() + ".tmp";
    std::ofstream output(temp, std::ios::trunc | std::ios::binary);
    if (!output)
        throw StateStoreError{"cannot open temporary state file: " + temp.string()};

    output << data.dump(2);
    output.close();
    if (!output)
        throw StateStoreError{"failed to write temporary state file: " + temp.string()};

    std::error_code error;
    std::filesystem::rename(temp, m_file, error);
    if (error)
        throw StateStoreError{"cannot replace state file: " + error.message()};
}

} // namespace lstu
