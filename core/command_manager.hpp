/* command_manager.hpp */

#pragma once

namespace optic_cli {
    enum state_codes_ {
        HELP_SIGNAL = 1,
        // More ...
    };

    // more functions above ...
    void display_help(bool is_error);
    bool match_code(state_codes_ _code_);
    state_codes_ read_args(const int argc, const char *const argv[]);
}