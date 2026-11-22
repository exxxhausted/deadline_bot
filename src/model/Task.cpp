#include "Task.hpp"

using namespace lstu;

TaskDraft::TaskDraft(std::int64_t id, std::int64_t chatId)
    : m_id(id), m_chatId(chatId) {}

TaskDraft& TaskDraft::setTitle(std::string t) {
    m_title = std::move(t);
    return *this;
}

TaskDraft& TaskDraft::setDeadline(Date d) {
    m_deadline = d;
    return *this;
}

TaskDraft& TaskDraft::setDescription(std::string n) {
    m_description = std::move(n);
    return *this;
}

Task TaskDraft::build() const {
    std::string title = m_title.value_or("");
    title.erase(0, title.find_first_not_of(" \t\r\n"));
    title.erase(title.find_last_not_of(" \t\r\n") + 1);

    if (title.empty())
        throw std::invalid_argument("не задано название");
    if (!m_deadline)
        throw std::invalid_argument("не задан дедлайн");
    if (!m_deadline->ok())
        throw std::invalid_argument("некорректный дедлайн");

    Task t;
    t.id          = m_id;
    t.chatId      = m_chatId;
    t.title       = std::move(title);
    t.deadline    = *m_deadline;
    t.description = m_description.value_or("");

    return t;
}

