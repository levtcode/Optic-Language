/* command_manager.hpp */

#pragma once

#include <unordered_map>

class OpticCLI {
private:
    void _init(int argc, char *argv[]);
    bool flag = false;

public:
    enum class Command {
        Help,
        Test,
        Install,
        Uninstall,
        Build,
        Run
    };

    OpticCLI(int argc, char *argv[]) { _init(--argc, ++argv); }

    // more functions above ...
    void display_help(bool is_error);
    inline bool bad_() { return flag; }

private:
    void dispatch_task(Command);

    const std::unordered_map<const char *, Command> command_table{
        {"help",      Command::Help},
        {"test",      Command::Test},
        {"install",   Command::Install},
        {"uninstall", Command::Uninstall},
        {"build",     Command::Build},
        {"run",       Command::Run},
    };
};