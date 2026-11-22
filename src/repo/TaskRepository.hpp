#ifndef TASKREPOSITORY_HPP
#define TASKREPOSITORY_HPP

#include <SQLiteCpp/Database.h>

#include "Task.hpp"
#include "DialogState.hpp"

namespace lstu {

class TaskRepository {
public:
    explicit TaskRepository(const std::string& dbPath);

    int64_t addTask(const Task& t);
    std::vector<Task> taskList(int64_t chatId);
    std::optional<DialogState> getState(std::int64_t chatId);
    void setState(std::int64_t chatId, DialogState state);

private:
    SQLite::Database m_db;
};

} // namespace lstu

#endif // TASKREPOSITORY_HPP
