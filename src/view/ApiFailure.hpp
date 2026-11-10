#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <cstdint>
#include <expected>
#include <string>

namespace lstu {

enum class ApiError {
    NetworkFailure,   // cpr::Response::error
    HttpError,        // status_code != 200
    InvalidJson,      // json::parse вернул discarded
    ApiNotOk,         // ok == false в ответе
    UnexpectedFormat  // нет ожидаемых полей
};

struct ApiFailure {
    ApiError code;
    std::int64_t http_status = 0; // 0, если HTTP-ответа не было
    std::string message;          // текст ошибки от cpr или сервера
};

template<typename T, typename E>
using Res = std::expected<T, E>;

} // namespace lstu

#endif // ERRORS_HPP
