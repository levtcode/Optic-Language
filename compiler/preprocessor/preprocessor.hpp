

/* preprocessor.hpp */

#pragma once
#include "../compiler_instance.hpp"

constexpr int SIGNAL_KEEP_GOING = 0x0;
constexpr int SIGNAL_END_ANALYSIS = 0xff;

class Macro {
    std::string name;
    std::string value;
    std::string expr;

public:

};

struct GraphNode {

};

class DependencyGraph {

};

class Preprocessor {
    SourceLocation loc;

    std::string read_preprocessor_keyword(File&);
    bool is_preprocessor_keyword(const std::string&);

    bool read_import(File&);
    void dependencies_analysis(File&, const DiagnosticEngine&);

public:
    int analyze(File &file, const DiagnosticEngine&);
};