#include "TelegramClient.hpp"
#include "BotStateMashine.hpp"

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
    std::int64_t marker = 0;
    BotStateMashine::start();
    BotStateMashine::client = client;

    while (true) {
        std::vector<Update> updates;
        try {
            updates = client->getUpdates(marker, 30);
        } catch (const std::exception& e) {
            std::cerr << "getUpdates: " << e.what() << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }

        for (const auto& u : updates) {
            if (u.update_id > 0)
                marker = u.update_id + 1;

            if (u.text.empty())
                continue;

            try {

                if (u.text == "/start")
                    BotStateMashine::dispatch(StartCommand{{}, u.chat_id});
                else if (u.text == "/reset")
                    BotStateMashine::reset();
                else if (u.text[0] != '/')
                    BotStateMashine::dispatch(MessageReceived{{}, u.chat_id, u.text});

            } catch (std::exception& e){
                std::cerr << "FSM:" << e.what() << '\n';
            }
        }
    }
}
