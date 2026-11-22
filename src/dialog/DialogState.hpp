#ifndef DIALOGSTATE_HPP
#define DIALOGSTATE_HPP

#include <string>
#include <string_view>

namespace lstu {

enum class State {
    Start = 0,
    Echo
};

std::string toString(State state);
State stateFromString(std::string_view name);

} // namespace lstu

#endif // DIALOGSTATE_HPP
