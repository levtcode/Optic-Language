/* command_manager.hpp */

#pragma once

#include <unordered_map>
#include <string_view>

namespace optic_cli {
    enum class CommandSignal {
        Continue,
        ShowHelp,
        ShowErrorHelp,
        // More ...
    };

    enum class Command {
        Help,
        Test,
        Install,
        Uninstall,
        Build,
        Run
    };

    const std::unordered_map<std::string_view, Command> command_table{
        {"help",      Command::Help},
        {"test",      Command::Test},
        {"install",   Command::Install},
        {"uninstall", Command::Uninstall},
        {"build",     Command::Build},
        {"run",       Command::Run},
    };

    // more functions above ...
    void display_help(bool is_error);
    void handle_signal(const CommandSignal signal);

    bool match_arg(const std::string_view &arg);

    CommandSignal read_args(const int argc, const char *const argv[]);
}


class CommandManager {
public:
    void _init();    
};