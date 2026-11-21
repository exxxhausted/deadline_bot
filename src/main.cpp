// Закомментируй эту строку, чтобы собрать бота с Telegram-клиентом.
#define CONSOLE_MODE

#ifdef CONSOLE_MODE
#include "ConsoleClient.hpp"
#else
#include "RetryingTelegramClient.hpp"
#endif

#include "BotDialogProcessor.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <limits>
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
    std::int64_t chatId = 0;
    std::cout << "Введите id чата: " << std::flush;
    if (!(std::cin >> chatId)) {
        std::cerr << "Некорректный id чата.\n";
        return 1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto client = std::make_shared<ConsoleClient>(chatId);
#else
    const char* tokenEnv = std::getenv("TELEGRAM_BOT_TOKEN");
    if (tokenEnv == nullptr || *tokenEnv == '\0') {
        std::cerr << "TELEGRAM_BOT_TOKEN не задан.\n"
                  << "Установите переменную окружения и перезапустите программу.\n";
        return 1;
    }
    auto client = std::make_shared<RetryingTelegramClient>(
        tokenEnv,
        3,
        std::chrono::milliseconds{250}
    );
#endif
    BotDialogProcessor proc(client, "stateStorage.json");
    std::int64_t marker = 0;

    constexpr int pollTimeoutSec = 30;
    constexpr auto retryDelay = std::chrono::seconds(3);

    while (true) {
        std::vector<Update> updates;
        try {
            updates = client->getUpdates(marker, pollTimeoutSec);
        } catch (const std::exception& e) {
            std::cerr << "getUpdates: " << e.what() << '\n';
            std::this_thread::sleep_for(retryDelay);
            continue;
        }

        for (const auto& update : updates) {
            try {
                proc.stateMachineStep(update);
                if (update.updateId + 1 > marker)
                    marker = update.updateId + 1;
            } catch (const std::exception& e) {
                std::cerr << "process update "
                          << update.updateId
                          << ": "
                          << e.what()
                          << '\n';
                std::this_thread::sleep_for(retryDelay);
                break;
            }
        }
    }
}
