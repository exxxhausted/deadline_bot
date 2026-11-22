#include "TaskRepository.hpp"

using namespace lstu;

TaskRepository::TaskRepository(std::filesystem::path dbPath)
    : m_db(dbPath.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    m_db.exec(R"sql(
        CREATE TABLE IF NOT EXISTS tasks (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            chat_id     INTEGER NOT NULL,
            title       TEXT    NOT NULL,
            deadline    TEXT    NOT NULL,   -- 'YYYY-MM-DD'
            description TEXT    NOT NULL DEFAULT '',
            done        INTEGER NOT NULL DEFAULT 0 CHECK (done IN (0, 1))
        );
        CREATE INDEX IF NOT EXISTS tasks_by_chat ON tasks (chat_id, done, deadline);
    )sql");
}

std::int64_t TaskRepository::add(const Task& task) {
    SQLite::Statement q(m_db, R"sql(
        INSERT INTO tasks (chat_id, title, deadline, description)
        VALUES (:chat_id, :title, :deadline, :description)
    )sql");
    q.bind(":chat_id", task.chatId);
    q.bind(":title", task.title);
    q.bind(":deadline", dateToIso(task.deadline));
    q.bind(":description", task.description);
    q.exec();
    return m_db.getLastInsertRowid();
}

std::vector<Task> TaskRepository::listActive(std::int64_t chatId) {
    SQLite::Statement q(m_db, R"sql(
        SELECT id, chat_id, title, deadline, description, done
        FROM tasks
        WHERE chat_id = :chat_id AND done = 0
        ORDER BY deadline, id
    )sql");
    q.bind(":chat_id", chatId);

    std::vector<Task> tasks;
    while (q.executeStep()) {
        tasks.push_back(Task{
            .id          = q.getColumn("id").getInt64(),
            .chatId      = q.getColumn("chat_id").getInt64(),
            .title       = q.getColumn("title").getString(),
            .deadline    = dateFromIso(q.getColumn("deadline").getString()),
            .description = q.getColumn("description").getString(),
            .done        = q.getColumn("done").getInt() != 0,
        });
    }
    return tasks;
}

