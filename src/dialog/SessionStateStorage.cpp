#include "SessionStateStorage.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <system_error>

namespace lstu {

using json = nlohmann::json;

SessionStateStorage::SessionStateStorage(std::filesystem::path file) : m_file(std::move(file)) {}

std::optional<State> SessionStateStorage::load(std::int64_t chatId) const {
    std::ifstream input(m_file);
    if (!input)
        throw StateFileNotFound{m_file};

    try {
        json data;
        input >> data;

        std::string key = std::to_string(chatId);

        auto stateIt = data.find(key);
        if (stateIt == data.end()) return std::nullopt;

        std::string name = stateIt->get<std::string>();

        try {
            return stateFromString(name);
        } catch (const std::invalid_argument&) {
            throw StateStorageError{"unknown state '" + name + "' for chat " + key};
        }

    } catch (const json::exception& error) {
        throw StateStorageError{"invalid state file " + m_file.string() + ": " + error.what()};
    }
}

void SessionStateStorage::saveAll(const std::unordered_map<std::int64_t, State>& sessions) const {
    if (m_file.has_parent_path()) {
        std::error_code error;
        std::filesystem::create_directories(m_file.parent_path(), error);
        if (error)
            throw StateStorageError{"cannot create state file directory: " + error.message()};
    }

    json data = json::object();

    for (const auto& [chatId, state] : sessions)
        data[std::to_string(chatId)] = toString(state);

    std::ofstream output(m_file, std::ios::trunc | std::ios::binary);
    if (!output)
        throw StateStorageError{"cannot open state file: " + m_file.string()};

    output << data.dump(2);
    if (!output)
        throw StateStorageError{"failed to write state file: " + m_file.string()};
}

} // namespace lstu
