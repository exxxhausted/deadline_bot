#ifndef RETRYINGTELEGRAMCLIENT_HPP
#define RETRYINGTELEGRAMCLIENT_HPP

#include "TelegramClient.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace lstu {

class RetryingTelegramClient final : public TelegramClient {
public:
    explicit RetryingTelegramClient(std::string token,
                                    int attempts,
                                    std::chrono::milliseconds retryDelay);

    std::vector<Update> getUpdates(std::int64_t marker, int timeoutSec) const override;
    void sendMessage(std::int64_t chatId, const std::string& text) const override;

private:
    const int m_attempts;
    const std::chrono::milliseconds m_retryDelay;
};

} // namespace lstu

#endif // RETRYINGTELEGRAMCLIENT_HPP
