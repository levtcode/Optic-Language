/* preprocessor.hpp */

#pragma once
#include "diagnostic_engine.hpp"

#include <cstddef>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <string_view>
#include <vector>

constexpr const char *optic_extension = ".optic";

struct Module {
    std::string module_name;
    std::filesystem::path module_path;

    Module(const std::string &fname) : module_path(fname) {
        module_name = module_path.filename().string();
        size_t ext = module_name.find_last_of('.');
        
        if (ext != std::string::npos)
            module_name.erase(ext);
    }
};

struct GraphNode {
    Module module;
    std::vector<std::string> dependencies;

    GraphNode(const std::string &n) : module(n) {}
};

class DependencyGraph {
    std::map<std::string, GraphNode> nodes;

    enum VisitState {
        Visited,
        Visiting
    };

    bool dfs(const GraphNode&, std::unordered_map<std::string, VisitState>&) const noexcept;

public:
    GraphNode& add_node(const std::string&);
    void add_dependency(const std::string&, const std::string&) noexcept;
    void resolve_dependencies();
    bool has_cycles() const noexcept;

#ifdef OPTIC_DEBUG
    void print_nodes();
#endif
};

class Macro {
    std::string name;
    std::string value;
    std::string expr;

public:
    // TODO
};

class Preprocessor {
    SourceLocation loc;
    Module module;
    DependencyGraph &graph;

    std::string get_module_name(const std::string_view&, size_t&);
    std::string read_keyword(const std::string_view&, size_t&);

public:
    Preprocessor(Module mod, DependencyGraph &g) :
        module(mod), graph(g) { loc.file = mod.module_name; }

    void analyze(std::string_view);
};