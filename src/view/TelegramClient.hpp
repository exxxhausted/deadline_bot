#ifndef TELEGRAMCLIENT_HPP
#define TELEGRAMCLIENT_HPP

#include "ApiFailure.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace lstu {

struct Update {
    std::int64_t update_id = 0;
    std::int64_t chat_id = 0;
    std::string text;
};

class TelegramClient {
public:
    explicit TelegramClient(std::string token);

    auto getUpdates(std::int64_t marker, int timeout_sec)
        -> Res<std::vector<Update>, ApiFailure>;

    auto sendMessage(std::int64_t chat_id, const std::string& text)
        -> Res<void, ApiFailure>;

private:
    std::string url(const std::string& method) const;

    std::string m_token;
};

} // namespace lstu

#endif // TELEGRAMCLIENT_HPP
