#pragma once
#include <string>
#include <chrono>
#include <cstdint>

namespace lstu {

    enum class TaskStatus {
        Planned,
        InProgress,
        Completed
    };

    struct Task {
        std::int64_t id;
        std::int64_t user_id;
        std::string title;
        std::chrono::year_month_day deadline;
        TaskStatus status;
        bool reminder_sent;
    };

} // namespace lstu