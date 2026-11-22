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

Task TaskDraft::build() {
    if (!m_title)
        throw std::invalid_argument("не задано название");

    m_title->erase(0, m_title->find_first_not_of(" \t\r\n"));
    m_title->erase(m_title->find_last_not_of(" \t\r\n") + 1);

    if (!m_deadline)
        throw std::invalid_argument("не задан дедлайн");
    if (!m_deadline->ok())
        throw std::invalid_argument("некорректный дедлайн");

    Task t;
    t.id          = m_id;
    t.chatId      = m_chatId;
    t.title       = *m_title;
    t.deadline    = *m_deadline;
    t.description = m_description.value_or("");

    return t;
}

