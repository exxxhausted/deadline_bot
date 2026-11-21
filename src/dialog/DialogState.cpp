#include "DialogState.hpp"

namespace lstu {

std::string_view toString(State state) {
    switch (state) {
    case State::Start:
        return "Start";
    case State::Echo:
        return "Echo";
    }
    return "Start";
}

std::optional<State> stateFromString(std::string_view name) {
    if (name == "Start")
        return State::Start;
    if (name == "Echo")
        return State::Echo;
    return std::nullopt;
}

} // namespace lstu
