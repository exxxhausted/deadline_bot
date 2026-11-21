#ifndef SESSIONSTATESTORE_HPP
#define SESSIONSTATESTORE_HPP

#include "DialogState.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <unordered_map>

namespace lstu {

class SessionStateStore {
public:
    explicit SessionStateStore(std::filesystem::path file = "bot_state.json");

    std::optional<State> load(std::int64_t chatId) const noexcept;
    void saveAll(const std::unordered_map<std::int64_t, State>& sessions) const noexcept;

private:
    std::filesystem::path m_file;
};

} // namespace lstu

#endif // SESSIONSTATESTORE_HPP
