#include "TelegramClient.hpp"
#include "ApiException.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <utility>

namespace lstu {

using json = nlohmann::json;

json parseResponseJson(const std::string& body) {
    try {
        return json::parse(body);
    } catch (const json::exception& error) {
        throw UnexpectedFormat{std::string{"invalid response JSON: "} + error.what()};
    }
}

TelegramClient::TelegramClient(std::string token) : m_token(std::move(token)) {}

std::string TelegramClient::url(const std::string& method) const {
    return "https://api.telegram.org/bot" + m_token + "/" + method;
}

std::vector<Update> TelegramClient::getUpdates(std::int64_t marker, int timeoutSec) const {
    cpr::Response response = cpr::Get(
        cpr::Url{url("getUpdates")},
        cpr::Parameters{
            {"offset", std::to_string(marker)},
            {"timeout", std::to_string(timeoutSec)}
        },
        cpr::Timeout{(timeoutSec + 5) * 1000}
        );

    if (response.error) {
        if (response.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT)
            throw ReachedTimeout{response.error.message};
        throw NetworkFailure{response.error.message};
    }

    if (response.status_code != 200)
        throw HttpError{response.status_code, response.text};

    json responseJson = parseResponseJson(response.text);

    if (!responseJson.value("ok", false))
        throw ApiNotOk{responseJson.value("description", std::string{})};

    if (!responseJson.contains("result"))
        throw UnexpectedFormat{"no result field"};

    std::vector<Update> updates;
    for (const auto& item : responseJson["result"]) {
        Update update;
        update.updateId = item.value("update_id", std::int64_t{0});
        if (item.contains("message")) {
            const auto& message = item["message"];
            if (message.contains("chat") && message.contains("text")) {
                update.chatId = message["chat"].value("id", std::int64_t{0});
                update.text = message["text"].get<std::string>();
            }
        }
        updates.push_back(std::move(update));
    }
    return updates;
}

void TelegramClient::sendMessage(std::int64_t chatId, const std::string& text) const {
    cpr::Response response = cpr::Post(
        cpr::Url{url("sendMessage")},
        cpr::Payload{{"chat_id", std::to_string(chatId)}, {"text", text}},
        cpr::Timeout{3000}
        );

    if (response.error) {
        if (response.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT)
            throw ReachedTimeout{response.error.message};
        throw NetworkFailure{response.error.message};
    }

    if (response.status_code != 200)
        throw HttpError{response.status_code, response.text};

    json responseJson = parseResponseJson(response.text);

    if (!responseJson.value("ok", false))
        throw ApiNotOk{responseJson.value("description", std::string{})};
}

} // namespace lstu
