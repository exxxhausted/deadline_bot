cmake_minimum_required(VERSION 3.24)
project(deadline_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.16.1
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs" FORCE)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

# основная логика; ни с чем не линкуется
#add_library(deadline_model STATIC
#
#)
#target_include_directories(deadline_model PUBLIC src/model)

# репозиторий; линкуется с SQLiteCpp, deadline_model
#add_library(deadline_repo STATIC
#    *внести сюда .cpp файлы из /src/repo, когда они появятся*
#)
#target_include_directories(deadline_repo PUBLIC src/repo)
#target_link_libraries(deadline_repo PRIVATE
#    deadline_model
#)

# коммуникация с ботом; линкуется с cpr, nlohmann_json
add_library(deadline_client STATIC
    src/client/TelegramClient.cpp
)
target_include_directories(deadline_client PUBLIC src/client)
target_link_libraries(deadline_client PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
)

# логика диалога; линкуется с deadline_model, deadline_repo, deadline_view
add_library(deadline_dialog
    src/dialog/BotStateMashine.cpp
)
target_include_directories(deadline_dialog PUBLIC src/dialog)
target_link_libraries(deadline_dialog PRIVATE
#    deadline_model
#    deadline_repo
    deadline_client
)

# исполняемый файл
add_executable(deadline_bot src/main.cpp)
target_link_libraries(deadline_bot PRIVATE
#    deadline_model
#    deadline_repo
    deadline_client
    deadline_dialog
)

# тестирование
#enable_testing()
#add_executable(deadline_tests
#)
#target_link_libraries(deadline_tests PRIVATE
#    deadline_model
#    deadline_repo
#    deadline_view
#    deadline_dialog
#    Catch2::Catch2WithMain
#)
#include(Catch)
#catch_discover_tests(deadline_tests)
