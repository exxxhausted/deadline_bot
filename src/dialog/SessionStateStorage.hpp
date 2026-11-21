#ifndef SESSIONSTATESTORE_HPP
#define SESSIONSTATESTORE_HPP

#include "DialogState.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace lstu {

class StateStorageError : public std::runtime_error {
public:
    explicit StateStorageError(const std::string& message) : std::runtime_error(message) {}
};

class StateFileNotFound final : public StateStorageError {
public:
    explicit StateFileNotFound(const std::filesystem::path& file)
        : StateStorageError("state file not found: " + file.string())
    {}
};

class SessionStateStorage {
public:
    explicit SessionStateStorage(std::filesystem::path file = "bot_state.json");

    std::optional<State> load(std::int64_t chatId) const;
    void saveAll(const std::unordered_map<std::int64_t, State>& sessions) const;

private:
    std::filesystem::path m_file;
};

} // namespace lstu

#endif // SESSIONSTATESTORE_HPP
