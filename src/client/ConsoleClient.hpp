#ifndef CONSOLECLIENT_HPP
#define CONSOLECLIENT_HPP

#include "IClient.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace lstu {

class ConsoleClient final : public IClient {
public:
    explicit ConsoleClient(std::int64_t chatId);

    std::vector<Update> getUpdates(std::int64_t marker, int timeoutSec) const override;
    void sendMessage(std::int64_t chatId, const std::string& text) const override;

    bool eof() const noexcept;

private:
    std::int64_t m_chatId;
    mutable std::int64_t m_nextUpdateId = 1;
    mutable bool m_eof = false;
};

} // namespace lstu

#endif // CONSOLECLIENT_HPP
