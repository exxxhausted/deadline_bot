cmake_minimum_required(VERSION 3.24)
project(deadline_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

include(FetchContent)

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
    src/client/RetryingTelegramClient.cpp
    src/client/ConsoleClient.cpp
)
target_include_directories(deadline_client PUBLIC src/client)
target_link_libraries(deadline_client PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
)

# логика диалога; линкуется с deadline_model, deadline_repo, deadline_view
add_library(deadline_dialog STATIC
    src/dialog/BotDialogProcessor.cpp
    src/dialog/DialogState.cpp
    src/dialog/SessionStateStorage.cpp
)
target_include_directories(deadline_dialog PUBLIC src/dialog)
target_link_libraries(deadline_dialog PRIVATE
#    deadline_model
#    deadline_repo
    deadline_client
    nlohmann_json::nlohmann_json
)

# исполняемый файл
add_executable(deadline_bot src/main.cpp)
target_link_libraries(deadline_bot PRIVATE
#    deadline_model
#    deadline_repo
    deadline_client
    deadline_dialog
)
