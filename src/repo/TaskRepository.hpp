#ifndef TASKREPOSITORY_HPP
#define TASKREPOSITORY_HPP

#include <filesystem>

#include <SQLiteCpp/Database.h>

#include "Task.hpp"

namespace lstu {

class TaskRepository {
public:
    explicit TaskRepository(std::filesystem::path dbPath);

    int64_t add(const Task& t);
    std::vector<Task> listActive(int64_t owner);

private:
    SQLite::Database m_db;
};

} // namespace lstu

#endif // TASKREPOSITORY_HPP
