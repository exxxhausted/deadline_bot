// Закомментируй эту строку, чтобы собрать бота с Telegram-клиентом.
//#define CONSOLE_MODE

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
#include <csignal>
#include <algorithm>

#define NOMINMAX

#ifdef _WIN32
#include <windows.h>
#endif

volatile std::sig_atomic_t g_stopRequested = 0; //
void onStopSignal(int) {                        // Инфраструктура механизма
    g_stopRequested = 1;                        // останова через ctrl+C
}                                               //

using namespace lstu;

int main() {

#ifdef _WIN32                       //
    SetConsoleOutputCP(CP_UTF8);    // Несчастная консоль
    SetConsoleCP(CP_UTF8);          // виндоуса
#endif                              //

#ifdef CONSOLE_MODE
    std::int64_t chatId = 0;
    std::cout << "Введите id чата: ";
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

    BotDialogProcessor proc(client,                         //
                            "repository.db");               //
    std::int64_t marker = 0;                                //
    std::signal(SIGINT, onStopSignal);                      // Инфраструктура бота
    constexpr int pollTimeoutSec = 30;                      //
    constexpr auto retryDelay = std::chrono::seconds(3);    //

    while (!g_stopRequested) {

        std::vector<Update> updates;                                //
        try {                                                       //
            updates = client->getUpdates(marker, pollTimeoutSec);   //
        } catch (const std::exception& e) {                         // Запрос у клиента
            std::cerr << "getUpdates: " << e.what() << '\n';        // обновлений во
            std::this_thread::sleep_for(retryDelay);                // всех чатах
            continue;                                               //
        }                                                           //

        for (const auto& update : updates) {
            if (g_stopRequested) break;

            try {
                proc.stateMachineStep(update);

                if (update.updateId + 1 > marker)
                    marker = update.updateId + 1;

            } catch (const std::exception& e) {
                std::cerr << "state mashine error on update №"
                          << update.updateId
                          << " : "
                          << e.what()
                          << '\n';

                std::this_thread::sleep_for(retryDelay);

                break;
            }
        }
    }

    try {                                                   //
        proc.saveSessions();                                // Сохранение состояний
    } catch (const std::exception& e) {                     // диалогов в файл
        std::cerr << "saveSessions: " << e.what() << '\n';  //
        return 1;                                           //
    }                                                       //

    return 0;
}
