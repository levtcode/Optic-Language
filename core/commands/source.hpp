/* source.hpp */

#pragma once

using command_handler = int(*)(int argc, char **argv);

int help_     (int argc, char **argv);
int test_     (int argc, char **argv);
int install_  (int argc, char **argv);
int uninstall_(int argc, char **argv);
int build_    (int argc, char **argv);
int run_      (int argc, char **argv);
int debug_    (int argc, char **argv);