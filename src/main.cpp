#include "TelegramClient.hpp"
#include "BotDialogProcessor.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace lstu;

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    const char* token_env = std::getenv("TELEGRAM_BOT_TOKEN");
    if (token_env == nullptr || *token_env == '\0') {
        std::cerr << "TELEGRAM_BOT_TOKEN не задан.\n"
                  << "Установите переменную окружения и перезапустите программу.\n";
        return 1;
    }
    const std::string token(token_env);


    auto client = std::make_shared<TelegramClient>(token);
    BotDialogState::client = client;
    BotDialogProcessor proc("test.json");
    std::int64_t marker = 0;

    while (true) {
        std::vector<Update> updates;
        try {
            updates = client->getUpdates(marker, 30);
        } catch (const std::exception& e) {
            std::cerr << "getUpdates: " << e.what() << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }

        if (!updates.empty())
            marker = updates.back().update_id + 1;

        proc.process(updates);
    }
}
