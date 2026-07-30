/* core.hpp */

#pragma once

/* */
class OpticCore {
private:
    void init_compiler();
public:
    [[nodiscard]] int run(int argc, char *argv[]);
};