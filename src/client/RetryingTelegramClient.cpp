#include "RetryingTelegramClient.hpp"
#include "ApiException.hpp"

#include <thread>
#include <utility>

namespace lstu {

namespace {

bool isRetryableHttpError(const HttpError& error) {
    return error.status == 429 || error.status >= 500;
}

} // namespace

RetryingTelegramClient::RetryingTelegramClient(std::string token,
                                               int attempts,
                                               std::chrono::milliseconds retryDelay)
    : TelegramClient(std::move(token)),
    m_attempts(attempts),
    m_retryDelay(retryDelay)
{}

std::vector<Update> RetryingTelegramClient::getUpdates(std::int64_t marker, int timeoutSec) const {
    for (int attempt = 1;; ++attempt) {
        try {
            return TelegramClient::getUpdates(marker, timeoutSec);
        } catch (const ReachedTimeout&) {
            throw;
        } catch (const NetworkFailure&) {
            if (attempt >= m_attempts)
                throw;
        } catch (const HttpError& error) {
            if (!isRetryableHttpError(error) || attempt >= m_attempts)
                throw;
        } catch (const UnexpectedFormat&) {
            if (attempt >= m_attempts)
                throw;
        }
        std::this_thread::sleep_for(m_retryDelay);
    }
}

void RetryingTelegramClient::sendMessage(std::int64_t chatId, const std::string& text) const {
    for (int attempt = 1;; ++attempt) {
        try {
            TelegramClient::sendMessage(chatId, text);
            return;
        } catch (const ReachedTimeout&) {
            throw;
        } catch (const NetworkFailure&) {
            if (attempt >= m_attempts)
                throw;
        } catch (const HttpError& error) {
            if (!isRetryableHttpError(error) || attempt >= m_attempts)
                throw;
        } catch (const UnexpectedFormat&) {
            if (attempt >= m_attempts)
                throw;
        }
        std::this_thread::sleep_for(m_retryDelay);
    }
}

} // namespace lstu
