#ifndef ICLIENT_HPP
#define ICLIENT_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace lstu {

struct Update {
    std::int64_t updateId = 0;
    std::int64_t chatId = 0;
    std::string text;
};

class IClient {
public:
    virtual ~IClient() = default;

    /**
     * @param marker идентификатор следующего ожидаемого обновления
     * @param timeoutSec максимальное время ожидания в секундах
     * @throws ApiException: ReachedTimeout, NetworkFailure, HttpError, ApiNotOk, UnexpectedFormat
     */
    virtual std::vector<Update> getUpdates(std::int64_t marker, int timeoutSec) const = 0;

    /**
     * @throws ApiException: ReachedTimeout, NetworkFailure, HttpError, ApiNotOk, UnexpectedFormat
     */
    virtual void sendMessage(std::int64_t chatId, const std::string& text) const = 0;
};

} // namespace lstu

#endif // ICLIENT_HPP
