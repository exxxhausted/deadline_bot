#include "DialogState.hpp"

#include <stdexcept>

namespace lstu {

std::string toString(State state) {
    switch (state) {
    case State::Start:
        return "Start";
    case State::Echo:
        return "Echo";
    }
    return "Start";
}

State stateFromString(std::string_view name) {
    if (name == "Start")
        return State::Start;
    if (name == "Echo")
        return State::Echo;

    throw std::invalid_argument("Cant convert \"" + std::string(name) + "\" to State");
}

} // namespace lstu
