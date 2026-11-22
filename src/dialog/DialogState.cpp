#include "DialogState.hpp"

#include <stdexcept>

namespace lstu {

std::string toString(DialogState state) {
    switch (state) {
    case DialogState::Start:
        return "Start";
    case DialogState::Echo:
        return "Echo";
    case DialogState::AwaitTitle:
        return "AwaitTitle";
    case DialogState::AwaitDeadline:
        return "AwaitDeadline";
    }
    return "Start";
}

DialogState stateFromString(std::string_view name) {
    if (name == "Start")
        return DialogState::Start;
    if (name == "Echo")
        return DialogState::Echo;
    if (name == "AwaitTitle")
        return DialogState::AwaitTitle;
    if (name == "AwaitDeadline")
        return DialogState::AwaitDeadline;

    throw std::invalid_argument("Cant convert \"" + std::string(name) + "\" to DialogState");
}

} // namespace lstu
