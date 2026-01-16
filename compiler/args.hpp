/* args.hpp */

#pragma once

#include "compiler_instance.hpp"
#include <string>

ArgValue get_arg_value(const std::string&, int, bool*, DiagnosticEngine&);
std::string get_option_name(const std::string&, size_t*);

void process_arg(const std::string&, CompilerInstance&);
void get_args(int, char **, CompilerInstance&);
void call_usage(bool=false);

#ifdef OPTIC_DEBUG
void print_args(CompilerArgs&);
#endif