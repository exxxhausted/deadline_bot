#ifndef TASK_HPP
#define TASK_HPP

#include "Date.hpp"

namespace lstu {

struct Task {
    std::int64_t id = 0;
    std::int64_t chatId = 0;
    std::string title;
    Date deadline{};
    std::string description;
    bool done = false;
};

class TaskDraft {
public:
    TaskDraft(std::int64_t id, std::int64_t chatId);
    TaskDraft& setOwner(std::int64_t id);
    TaskDraft& setTitle(std::string t);
    TaskDraft& setDeadline(Date d);
    TaskDraft& setDescription(std::string n);

    Task build();

private:
    std::int64_t m_id;
    std::int64_t m_chatId;
    std::optional<std::int64_t> m_owner;
    std::optional<std::string> m_title;
    std::optional<Date> m_deadline;
    std::optional<std::string> m_description;
};

} // namespace lstu

#endif // TASK_HPP
