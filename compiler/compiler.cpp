#include <fstream>
#include <array>
#include <iostream>
#include <algorithm>

const std::array<std::string, 2> options = {
    "create_binary_disk",
    "disable_garbage_collector",
    // TODO
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Fatal error: No input files\n";
        return EXIT_FAILURE;
    }

    std::string_view sv;
    std::string file_name;

    for (int i = 1; i < argc; i++) {
        sv = argv[i];
        auto it = std::find(options.begin(), options.end(), sv);

        if (it != options.end()) {
            // TODO
        }

        else file_name = sv;
    }

    std::ifstream f(file_name);

    if (!f.is_open()) {
        std::cout << "Error: Cant open file '" << file_name << "' Does not exists!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}