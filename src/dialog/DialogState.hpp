#ifndef DIALOGSTATE_HPP
#define DIALOGSTATE_HPP

#include <optional>
#include <string_view>

namespace lstu {

enum class State {
    Start = 0,
    Echo
};

std::string_view toString(State state);
std::optional<State> stateFromString(std::string_view name);

} // namespace lstu

#endif // DIALOGSTATE_HPP
