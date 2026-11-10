#include "TelegramClient.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace lstu;

int main(int argc, char* argv[]) {

    const char* token_env = std::getenv("TELEGRAM_BOT_TOKEN");
    if (token_env == nullptr || *token_env == '\0') {
        std::cerr << "TELEGRAM_BOT_TOKEN не задан.\n"
                  << "Установите переменную окружения и перезапустите программу.\n";
        return 1;
    }
    const std::string token = token_env;

    TelegramClient client(token);
    std::int64_t marker = 0;

    while (true) {
        auto result = client.getUpdates(marker, 30);

        if (!result) {
            const auto& err = result.error();
            std::cerr << "getUpdates: code=" << static_cast<int>(err.code)
                      << " http=" << err.http_status
                      << " msg=" << err.message << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }

        for (const auto& u : *result) {
            if (!u.text.empty())
                if (auto sent = client.sendMessage(u.chat_id, u.text); !sent)
                    std::cerr << "sendMessage: " << sent.error().message << '\n';

            if (u.update_id > 0)
                marker = u.update_id + 1;
        }
    }
}
