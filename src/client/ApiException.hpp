#ifndef APIEXCEPTION_HPP
#define APIEXCEPTION_HPP

#include <cstdint>
#include <stdexcept>
#include <string>

namespace lstu {

class ApiException : public std::runtime_error {
public:
    explicit ApiException(const std::string& message) : std::runtime_error(message) {}
};

class NetworkFailure final : public ApiException {
public:
    explicit NetworkFailure(const std::string& message) : ApiException(message) {}
};

class HttpError final : public ApiException {
public:
    HttpError(std::int64_t status, const std::string& body)
        : ApiException("HTTP " + std::to_string(status) + ": " + body),
        status(status),
        body(body)
    {}

    std::int64_t status;
    std::string body;
};

class ApiNotOk final : public ApiException {
public:
    explicit ApiNotOk(const std::string& description) : ApiException(description) {}
};

class UnexpectedFormat final : public ApiException {
public:
    explicit UnexpectedFormat(const std::string& message) : ApiException(message) {}
};

} // namespace lstu

#endif // APIEXCEPTION_HPP
