#ifndef CONSOLECLIENT_HPP
#define CONSOLECLIENT_HPP

#include "IClient.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace lstu {

class ConsoleClient final : public IClient {
public:
    std::vector<Update> getUpdates(std::int64_t marker, int timeoutSec) const override;
    void sendMessage(std::int64_t chatId, const std::string& text) const override;

private:
    mutable std::int64_t m_nextUpdateId = 1;
};

} // namespace lstu

#endif // CONSOLECLIENT_HPP
