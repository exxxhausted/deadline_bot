#include "TelegramClient.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <utility>

namespace lstu {

using json = nlohmann::json;

TelegramClient::TelegramClient(std::string token) : m_token(std::move(token)) {}

std::string TelegramClient::url(const std::string& method) const {
    return "https://api.telegram.org/bot" + m_token + "/" + method;
}

auto TelegramClient::getUpdates(std::int64_t marker, int timeout_sec)
    -> Res<std::vector<Update>, ApiFailure>
{
    cpr::Response r = cpr::Get(
            cpr::Url{url("getUpdates")},
            cpr::Parameters{
                {"offset", std::to_string(marker)},
                {"timeout", std::to_string(timeout_sec)}
            },
            cpr::Timeout{(timeout_sec + 10) * 1000}
        );

    if (r.error)
        return std::unexpected(ApiFailure{
            ApiError::NetworkFailure,
            0,
            r.error.message
        });

    if (r.status_code != 200)
        return std::unexpected(ApiFailure{
            ApiError::HttpError,
            r.status_code,
            r.text
        });

    json j = json::parse(r.text, nullptr, false);
    if (j.is_discarded())
        return std::unexpected(ApiFailure{
            ApiError::InvalidJson,
            r.status_code,
            "parse failed"
        });

    if (!j.value("ok", false))
        return std::unexpected(ApiFailure{
            ApiError::ApiNotOk,
            r.status_code,
            j.value("description", std::string{})
        });

    if (!j.contains("result"))
        return std::unexpected(ApiFailure{
            ApiError::UnexpectedFormat,
            r.status_code,
            "no result field"
        });


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

auto TelegramClient::sendMessage(std::int64_t chat_id, const std::string& text)
    -> Res<void, ApiFailure>
{
    cpr::Response r = cpr::Post(
            cpr::Url{url("sendMessage")},
            cpr::Payload{{"chat_id", std::to_string(chat_id)}, {"text", text}},
            cpr::Timeout{10000}
        );

    if (r.error)
        return std::unexpected(ApiFailure{
            ApiError::NetworkFailure,
            0,
            r.error.message
        });

    if (r.status_code != 200)
        return std::unexpected(ApiFailure{
            ApiError::HttpError,
            r.status_code,
            r.text
        });

    json j = json::parse(r.text, nullptr, false);
    if (j.is_discarded())
        return std::unexpected(ApiFailure{
            ApiError::InvalidJson,
            r.status_code,
            "parse failed"
        });

    if (!j.value("ok", false))
        return std::unexpected(ApiFailure{
            ApiError::ApiNotOk,
            r.status_code,
            j.value("description", std::string{})
        });

    return {};
}

} // namespace lstu
