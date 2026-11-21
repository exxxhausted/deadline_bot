#include "SessionStateStore.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <utility>

namespace lstu {

namespace {

constexpr int kSchemaVersion = 1;

void warn(const std::string& message) {
    std::cerr << "SessionStateStore: " << message << '\n';
}

} // namespace

using json = nlohmann::json;

SessionStateStore::SessionStateStore(std::filesystem::path file) : m_file(std::move(file)) {}

std::optional<State> SessionStateStore::load(std::int64_t chatId) const noexcept {
    try {
        std::ifstream input(m_file);
        if (!input)
            return std::nullopt;

        json data;
        try {
            input >> data;
        } catch (const json::exception& error) {
            warn(std::string{"failed to parse "} + m_file.string() + ": " + error.what());
            return std::nullopt;
        }

        if (!data.is_object()) {
            warn("root is not an object");
            return std::nullopt;
        }

        if (data.value("version", 0) != kSchemaVersion) {
            warn("unsupported schema version");
            return std::nullopt;
        }

        const auto sessionsIt = data.find("sessions");
        if (sessionsIt == data.end() || !sessionsIt->is_object())
            return std::nullopt;

        const std::string key = std::to_string(chatId);
        const auto stateIt = sessionsIt->find(key);
        if (stateIt == sessionsIt->end() || !stateIt->is_string())
            return std::nullopt;

        const std::string name = stateIt->get<std::string>();
        const auto state = stateFromString(name);
        if (!state)
            warn("unknown state for chat " + key + ": " + name);
        return state;
    } catch (const std::exception& error) {
        warn(std::string{"load failed: "} + error.what());
        return std::nullopt;
    }
}

void SessionStateStore::saveAll(const std::unordered_map<std::int64_t, State>& sessions) const noexcept {
    try {
        if (m_file.has_parent_path()) {
            std::error_code error;
            std::filesystem::create_directories(m_file.parent_path(), error);
            if (error) {
                warn("cannot create directory: " + error.message());
                return;
            }
        }

        json serializedSessions = json::object();
        for (const auto& [chatId, state] : sessions)
            serializedSessions[std::to_string(chatId)] = std::string{toString(state)};

        json data;
        data["version"] = kSchemaVersion;
        data["sessions"] = std::move(serializedSessions);

        const std::filesystem::path temp = m_file.string() + ".tmp";
        std::ofstream output(temp, std::ios::trunc | std::ios::binary);
        if (!output) {
            warn("cannot open temporary file for writing");
            return;
        }
        output << data.dump(2);
        output.close();
        if (!output) {
            warn("failed to write temporary file");
            return;
        }

        std::error_code error;
        std::filesystem::rename(temp, m_file, error);
        if (error)
            warn("cannot replace state file: " + error.message());
    } catch (const std::exception& error) {
        warn(std::string{"save failed: "} + error.what());
    }
}

} // namespace lstu
