#ifndef DIALOGSTATE_HPP
#define DIALOGSTATE_HPP

#include <string>
#include <string_view>

namespace lstu {

enum class DialogState {
    Start = 0,
    Echo,
    AwaitTitle,
    AwaitDeadline
};

std::string toString(DialogState state);
DialogState stateFromString(std::string_view name);

} // namespace lstu

#endif // DIALOGSTATE_HPP
