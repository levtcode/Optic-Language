/* core.hpp */

#pragma once

#include <unordered_map>
#include <string_view>

/* */
class OpticCore {
public:
    enum class Command {
        Help,
        Test,
        Install,
        Uninstall,
        Build,
        Run,
        Debug,
        Count
    };

private:
    std::unordered_map<std::string_view, Command> command_table {
        {"help",      Command::Help},
        {"test",      Command::Test},
        {"install",   Command::Install},
        {"uninstall", Command::Uninstall},
        {"build",     Command::Build},
        {"run",       Command::Run},
        {"debug",     Command::Debug}
    };

    Command get_command(const char *current_arg);
    int dispatch_command(Command, int argc, char **);

public:
    [[nodiscard]] int run(int argc, char *argv[]);
};