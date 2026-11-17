#ifndef TELEGRAMCLIENT_HPP
#define TELEGRAMCLIENT_HPP

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

    /**
     * @throws ApiException: NetworkFailure, HttpError, ApiNotOk, UnexpectedFormat
     * @throws nlohmann::json::parse_error при некорректном JSON в ответе
     */
    std::vector<Update> getUpdates(std::int64_t marker, int timeout_sec);

    /**
     * @throws ApiException: NetworkFailure, HttpError, ApiNotOk
     * @throws nlohmann::json::parse_error при некорректном JSON в ответе
     */
    void sendMessage(std::int64_t chat_id, const std::string& text);

private:
    std::string url(const std::string& method) const;

    std::string m_token;
};

} // namespace lstu

#endif // TELEGRAMCLIENT_HPP
