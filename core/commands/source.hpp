/* source.hpp */

#pragma once

using command_handler = int(*)(int argc, char **argv);

int test_     (int argc, char **argv);  // test.cpp
int install_  (int argc, char **argv);  // lib_manager.cpp
int uninstall_(int argc, char **argv);  // lib_manager.cpp
int build_    (int argc, char **argv);  // execution.cpp
int run_      (int argc, char **argv);  // execution.cpp
int debug_    (int argc, char **argv);  // execution.cpp