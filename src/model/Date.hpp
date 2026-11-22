#ifndef DATE_HPP
#define DATE_HPP

#include <chrono>

namespace lstu {

using Date = std::chrono::year_month_day;

Date dateFromIso(std::string_view s);
std::string dateToIso(Date d);

} // namespace lstu

#endif // DATE_HPP
