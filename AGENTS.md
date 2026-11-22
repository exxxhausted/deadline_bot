# AGENTS.md

## Where to work

- Work from the repository root (`git rev-parse --show-toplevel`), not from an external worktree.
- `master` may already be checked out in another worktree. If `git switch master` fails with "already used by worktree", do not fight it: run `git fetch origin master` and create the stage branch from `origin/master`.
- When the user asks for a plan, patch, explanation, or code "in this chat", do not create branches and do not edit files. Answer in chat only.
- Before editing, create a fresh branch from an up-to-date `master`:
  `git switch master && git pull --ff-only && git switch -c agent/<short-topic>`.
  Do not commit directly to `master`.

## Build and verification

- Do not run builds or tests unless the user explicitly asks: a fresh FetchContent configure can take several minutes.
- No README, no CI, no test sources. `CMakeLists.txt` is the only build source of truth; C++20.
- Targets: `deadline_client` (`TelegramClient.cpp` + `RetryingTelegramClient.cpp` + `ConsoleClient.cpp`), `deadline_dialog` (`BotDialogProcessor.cpp` + `DialogState.cpp` + `SessionStateStorage.cpp`), `deadline_bot` (`src/main.cpp`).
- Pinned FetchContent deps: cpr `1.10.5`, nlohmann/json `v3.11.3`. Fresh configure needs network. Build directories are gitignored (`build*/`) and may be stale — never rely on a machine-specific path.
- Static builds are required on both toolchains: MSVC via `CMAKE_MSVC_RUNTIME_LIBRARY` (`/MT`, `/MTd`), MinGW via `-static -static-libgcc -static-libstdc++`. Keep both when touching `CMakeLists.txt`.

## Client mode

- Mode switch is a single `#define CONSOLE_MODE` at the top of `src/main.cpp`, **not** a CMake option. Comment it out to build with `RetryingTelegramClient`.
- Console mode must not require a token: `TELEGRAM_BOT_TOKEN` is read only in the Telegram `#else` branch.
- `ConsoleClient` is console-only: reads `std::cin`, writes `std::cout`, no injected streams. It stores the chat id entered at startup and assigns it to every `Update`; `sendMessage` prints `[chatId] text`.
- `getUpdates` reads one line, ignores `timeoutSec`, and returns an empty vector on EOF while setting `eof()`. Console mode exits when EOF is reached.

## Code conventions and gotchas

- `IClient` is in `src/client/IClient.hpp` and also defines `Update`. Both clients inherit it; its methods are `const`.
- Naming: members `m_` + camelCase (`m_nextUpdateId`), parameters/locals camelCase (`chatId`, `timeoutSec`). Telegram JSON keys stay snake_case (`update_id`, `chat_id`).
- `ApiException.hpp` is included by `TelegramClient.cpp` and `RetryingTelegramClient.cpp`; include it explicitly where API exceptions are thrown or caught.
- `DialogState.hpp` owns the serializable `State` enum; `SessionStateStorage` reads/writes the state file.
- `SessionStateStorage::load()` returns `nullopt` only when the file is valid but has no record for the chat. Missing/corrupt files throw `StateStorageError` / `StateFileNotFound`. The storage never logs or swallows errors.
- `SessionStateStorage::saveAll()` must merge into the existing file. It must not drop chats that were not seen during the current process lifetime.
- `BotDialogProcessor::stateMachineStep()` currently calls `restoreSession()` and `saveSessions()` directly as a temporary solution.
- Prefer the smallest working solution. Do not add versioning, temp-file atomic writes, generalized stop-signal classes, SIGTERM/CTRL_CLOSE handling, or defensive layers unless the user explicitly asks.
- Graceful shutdown is intentionally limited to Ctrl+C via `std::signal(SIGINT, ...)` and a `volatile std::sig_atomic_t` flag. Do not add a general `StopSignal` abstraction.
