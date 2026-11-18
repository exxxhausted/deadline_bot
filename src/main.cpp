// Закомментируйте эту строку, чтобы собрать бота с Telegram-клиентом.
#define CONSOLE_MODE

#ifdef CONSOLE_MODE
#include "ConsoleClient.hpp"
#else
#include "TelegramClient.hpp"
#endif

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

#ifdef CONSOLE_MODE
    auto client = std::make_shared<ConsoleClient>();
#else
    const char* tokenEnv = std::getenv("TELEGRAM_BOT_TOKEN");
    if (tokenEnv == nullptr || *tokenEnv == '\0') {
        std::cerr << "TELEGRAM_BOT_TOKEN не задан.\n"
                  << "Установите переменную окружения и перезапустите программу.\n";
        return 1;
    }
    auto client = std::make_shared<TelegramClient>(tokenEnv);
#endif
    BotDialogProcessor proc(client);
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
            marker = updates.back().updateId + 1;

        proc.process(updates);
    }
}
