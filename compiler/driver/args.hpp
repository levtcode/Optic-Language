/* args.hpp */

#pragma once

#include "compiler_instance.hpp"
#include <string>

void print_args_files(std::vector<std::string>&);
void print_args_options(std::unordered_map<std::string, ArgValue>&);
void print_args_flags(std::unordered_map<std::string, bool>&);
void print_args(CompilerArgs&);