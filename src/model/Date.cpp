#include "Date.hpp"

#include <regex>

using namespace lstu;

static const std::regex dateFormat(R"((\d{2})-(\d{2})-(\d{4}))");

Date lstu::dateFromIso(std::string_view s) {
    const std::string str(s);

    std::smatch m;
    if (std::regex_match(str, m, dateFormat)) {
        const Date date{std::chrono::year{std::stoi(m[3].str())},
                        std::chrono::month{static_cast<unsigned>(std::stoi(m[2].str()))},
                        std::chrono::day{static_cast<unsigned>(std::stoi(m[1].str()))}};

        if (date.ok())
            return date;
    }

    throw std::invalid_argument("некорректная дата: " + str);
}

std::string lstu::dateToIso(Date d) {
    if (!d.ok()) throw std::invalid_argument("некорректная дата (нельзя преобразовать в строку)");

    char buf[16];
    std::snprintf(buf, sizeof buf, "%02u-%02u-%04d",
                  static_cast<unsigned>(d.day()),
                  static_cast<unsigned>(d.month()),
                  static_cast<int>(d.year()));
    return buf;
}
