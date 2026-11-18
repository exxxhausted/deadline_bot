#include "ConsoleClient.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace lstu {

std::vector<Update> ConsoleClient::getUpdates(std::int64_t marker, int /*timeoutSec*/) const {
    // marker приходит от вызывающей стороны как updateId последнего
    // обработанного сообщения + 1; не выдаём идентификаторы меньше него.
    m_nextUpdateId = std::max(m_nextUpdateId, marker);

    std::string line;
    if (!std::getline(std::cin, line))
        return {};

    Update update;
    update.updateId = m_nextUpdateId++;
    update.text = std::move(line);

    return {std::move(update)};
}

void ConsoleClient::sendMessage(std::int64_t chatId, const std::string& text) const {
    std::cout << '[' << chatId << "] " << text << '\n' << std::flush;
}

} // namespace lstu
