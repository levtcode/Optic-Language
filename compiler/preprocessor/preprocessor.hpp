/* preprocessor.hpp */

#pragma once
#include "diagnostic_engine.hpp"

#include <cstddef>
#include <filesystem>
#include <map>
#include <vector>
#include <unordered_map>

/* */
struct Module {
    std::string module_name;
    std::filesystem::path module_path;

    Module() = default;
    Module(const std::string &fname) : module_path(fname) {
        module_name = module_path.filename().string();
        size_t ext = module_name.find_last_of('.');

        if (ext != std::string::npos)
            module_name.erase(ext);
    }
};

/* */
struct PreprocessedModule {
    std::string module;
    std::vector<uint8_t> buffer;

    PreprocessedModule() = default;
    PreprocessedModule(const std::string &mod, const std::vector<uint8_t> &vec) : module(mod), buffer(vec) {}
};

/* */
struct GraphNode {
    std::string module;
    std::vector<std::string> dependencies;

    GraphNode(const std::string &mod) : module(mod) {}
};

/* */
class DependencyGraph {
    std::map<std::string, GraphNode> nodes;
    std::vector<std::string> compilation_order;

    bool dfs_search_cycles() const noexcept;

public:
    enum VisitState {
        Visited,
        Visiting
    };

    GraphNode& add_node(const std::string&);
    void add_dependency(const std::string&, const std::string&) noexcept;
    void topological_sort() noexcept;
    const std::vector<std::string>& get_order() const noexcept { return compilation_order; }

#ifdef OPTIC_DEBUG
    void print_nodes();
#endif
};

/* */
class Macro {
    std::string name;
    std::string value;
    std::string expr;

public:
    // TODO
};

/* */
struct PreprocessorContext {
    DiagnosticEngine *diagnostic_engine;
    DependencyGraph *graph;

    PreprocessorContext(DiagnosticEngine *diag_engine, DependencyGraph *g) : diagnostic_engine(diag_engine), graph(g) {}
};

/* */
class Preprocessor {
    PreprocessorContext preprocessor_context;

    void skip_comments(std::vector<uint8_t>&, size_t&, SourceLocation&);
    void include_module(const Module&, std::vector<uint8_t>&, size_t&, SourceLocation&);

    std::string get_module_name(const std::vector<uint8_t>&, size_t&, SourceLocation&);
    std::string read_keyword(const std::vector<uint8_t>&, size_t&, SourceLocation&);

public:
    Preprocessor(const PreprocessorContext &pr_ctx) : preprocessor_context(pr_ctx) {}

    PreprocessedModule analyze(const std::string&, std::vector<uint8_t>&);
    void sort_modules() noexcept { preprocessor_context.graph->topological_sort(); }

#ifdef OPTIC_DEBUG
    void print_modules() noexcept { preprocessor_context.graph->print_nodes(); }
#endif

    void combine_modules(std::vector<uint8_t>&, const std::unordered_map<std::string, PreprocessedModule>&);
};