#ifndef TELEGRAMCLIENT_HPP
#define TELEGRAMCLIENT_HPP

#include "IClient.hpp"

#include <string>
#include <vector>

namespace lstu {

class TelegramClient : public IClient {
public:

    explicit TelegramClient(std::string token);

    /**
     * @throws ApiException: NetworkFailure, HttpError, ApiNotOk, UnexpectedFormat
     * @throws nlohmann::json::parse_error при некорректном JSON в ответе
     */
    std::vector<Update> getUpdates(std::int64_t marker, int timeoutSec) const override;

    /**
     * @throws ApiException: NetworkFailure, HttpError, ApiNotOk
     * @throws nlohmann::json::parse_error при некорректном JSON в ответе
     */
    void sendMessage(std::int64_t chatId, const std::string& text) const override;

private:

    std::string url(const std::string& method) const;

    std::string m_token;
};

} // namespace lstu

#endif // TELEGRAMCLIENT_HPP
