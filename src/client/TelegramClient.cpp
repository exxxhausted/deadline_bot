#include "TelegramClient.hpp"
#include "ApiException.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <utility>

namespace lstu {

using json = nlohmann::json;

TelegramClient::TelegramClient(std::string token) : m_token(std::move(token)) {}

std::string TelegramClient::url(const std::string& method) const {
    return "https://api.telegram.org/bot" + m_token + "/" + method;
}

std::vector<Update> TelegramClient::getUpdates(std::int64_t marker, int timeout_sec) {
    cpr::Response r = cpr::Get(
        cpr::Url{url("getUpdates")},
        cpr::Parameters{
            {"offset", std::to_string(marker)},
            {"timeout", std::to_string(timeout_sec)}
        },
        cpr::Timeout{(timeout_sec + 10) * 1000}
        );

    if (r.error)
        throw NetworkFailure{r.error.message};

    if (r.status_code != 200)
        throw HttpError{r.status_code, r.text};

    json j = json::parse(r.text);

    if (!j.value("ok", false))
        throw ApiNotOk{j.value("description", std::string{})};

    if (!j.contains("result"))
        throw UnexpectedFormat{"no result field"};

    std::vector<Update> updates;
    for (const auto& item : j["result"]) {
        Update u;
        u.update_id = item.value("update_id", std::int64_t{0});
        if (item.contains("message")) {
            const auto& m = item["message"];
            if (m.contains("chat") && m.contains("text")) {
                u.chat_id = m["chat"].value("id", std::int64_t{0});
                u.text = m["text"].get<std::string>();
            }
        }
        updates.push_back(std::move(u));
    }
    return updates;
}

void TelegramClient::sendMessage(std::int64_t chat_id, const std::string& text) {
    cpr::Response r = cpr::Post(
        cpr::Url{url("sendMessage")},
        cpr::Payload{{"chat_id", std::to_string(chat_id)}, {"text", text}},
        cpr::Timeout{10000}
        );

    if (r.error)
        throw NetworkFailure{r.error.message};

    if (r.status_code != 200)
        throw HttpError{r.status_code, r.text};

    json j = json::parse(r.text);

    if (!j.value("ok", false))
        throw ApiNotOk{j.value("description", std::string{})};
}

} // namespace lstu
