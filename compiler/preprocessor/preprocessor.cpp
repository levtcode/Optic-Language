/* preprocessor.cpp */

#include "preprocessor.hpp"

#include <cctype>
#include <iostream>
#include <cstddef>

std::vector<Macro> macros;	// SOON: Dont let 'macros' variable be global, use a local variable instead

// ========== Dependency Graph methods ==========

#ifdef OPTIC_DEBUG

/* */
void DependencyGraph::print_nodes() {
	std::cout << "----- PRINTING GRAPH NODES -----\n\n";

	for (auto &[module_name, node] : nodes) {
		std::cout << "\nModule name: " << module_name << "\n";
		
		std::cout << "Dependencies = [";
		for (size_t i = 0; i < node.dependencies.size(); i++) {
			std::cout << node.dependencies[i];
			
			if (i + 1 < node.dependencies.size())
				std::cout << ", ";
		}
		std::cout << "]\n\n";
	}
}

#endif

/* */
bool dfs_recursive(const GraphNode &node, const std::map<std::string, GraphNode> &nodes, std::unordered_map<std::string, DependencyGraph::VisitState> &visit) noexcept {
	if (visit.contains(node.module)) {
		if (visit[node.module] == DependencyGraph::Visiting)
			return true;
		else if (visit[node.module] == DependencyGraph::Visited)
			return false;
	}

	visit[node.module] = DependencyGraph::Visiting;

	for (auto &dep : node.dependencies) {
		auto dep_it = nodes.find(dep);
		if (dfs_recursive(dep_it->second, nodes, visit)) return true;
	}

	visit[node.module] = DependencyGraph::Visited;
	return false;
}

/* */
void dfs_topo(GraphNode &node, std::map<std::string, GraphNode> &nodes, std::unordered_map<std::string, DependencyGraph::VisitState> &visit, std::vector<std::string> &order) noexcept {
	if (visit.contains(node.module)) {
		if (visit[node.module] == DependencyGraph::Visited)
			return;
	}

	visit[node.module] = DependencyGraph::Visiting;

	for (auto &dep : node.dependencies) {
		auto dep_it = nodes.find(dep);
		dfs_topo(dep_it->second, nodes, visit, order);
	}

	visit[node.module] = DependencyGraph::Visited;
	order.push_back(node.module);
}

/* */
GraphNode& DependencyGraph::add_node(const std::string &module) {
	auto it = nodes.try_emplace(module, module);
	return it.first->second;
}

/* */
void DependencyGraph::add_dependency(const std::string &module, const std::string &dependency) noexcept {
	auto &node = add_node(module);
	node.dependencies.push_back(dependency);
	add_node(dependency);

	if (dfs_search_cycles()) {
		nodes.erase(node.module);

		// LATER: if the 'Wall' flag is on, report a warning for graph cycles
	}
}

/* */
void DependencyGraph::topological_sort() noexcept {
	std::unordered_map<std::string, VisitState> visit;

	for (auto &[_, node] : nodes) {
		if (!visit.contains(node.module)) {
			dfs_topo(node, nodes, visit, compilation_order);
		}
	}
}

/* */
bool DependencyGraph::dfs_search_cycles() const noexcept {
	std::unordered_map<std::string, VisitState> visit;
	
	for (auto &[_, node] : nodes) {
		if (!visit.contains(node.module))
			if (dfs_recursive(node, nodes, visit)) return true;
	}

	return false;
}

// ========== Preprocessor methods ==========

/* */
std::string Preprocessor::get_module_name(const std::vector<uint8_t> &buffer, size_t &cursor, SourceLocation &loc) {
	std::string module_;

	while (cursor < buffer.size() && !isspace(buffer[cursor])) {
		module_ += buffer[cursor++];
		loc.column++;
	}

	if (cursor < buffer.size() && buffer[cursor] == '\n') {
		loc.line++;
		loc.column = 1;
		cursor++;
	}

	return module_;
}

/* */
std::string Preprocessor::read_keyword(const std::vector<uint8_t> &buffer, size_t &cursor, SourceLocation &loc) {
	std::string keyword;

	while (cursor < buffer.size() && isalpha(buffer[cursor])) {
		keyword += buffer[cursor++];
		loc.column++;
	}

	return keyword;
}

void Preprocessor::skip_comments(std::vector<uint8_t> &buffer, size_t &cursor, SourceLocation &loc) {
	while (cursor < buffer.size()) {
		if (buffer[cursor] == '/') {
			cursor++;
			if (cursor < buffer.size()) break;

			if (buffer[cursor] == '/') {
				while ((cursor < buffer.size()) && (buffer[cursor] != '\n')) cursor++;

				if (cursor < buffer.size()) {
					loc.line++;
					loc.column = 1;
				}

			} else if (buffer[cursor] == '*') {
				cursor++;
				
				while ((cursor < buffer.size() - 1) && !(buffer[cursor] == '*' && buffer[cursor + 1] == '/')) {
					if (buffer[cursor] == '\n') {
						loc.line++;
						loc.column = 1;
					}
					cursor++;
				}
			}

			break;
		}
		
		break;
	}
}

void Preprocessor::include_module(const Module &mod, std::vector<uint8_t> &buffer, size_t &cursor, SourceLocation &loc) {
	while (cursor < buffer.size() && isspace(buffer[cursor]) && buffer[cursor] != '\n') cursor++;
	std::string dep_name = get_module_name(buffer, cursor, loc);
	preprocessor_context.graph->add_dependency(mod.module_name, dep_name);
}

/* */
PreprocessedModule Preprocessor::analyze(const std::string &mod_name, std::vector<uint8_t> &buffer) {
	PreprocessedModule module(mod_name, buffer);
	SourceLocation loc;
	size_t cursor = 0;
	
	loc.file = mod_name;

	while (cursor < buffer.size()) {
		skip_comments(buffer, cursor, loc);

		if (buffer[cursor] == '\n') {
			loc.line++;
			loc.column = 1;
			cursor++;
			continue;
		}

		if (isalpha(buffer[cursor])) {
			size_t start_ = cursor;
			std::string keyword = read_keyword(buffer, cursor, loc);

			if (keyword == "import") {
				include_module(mod_name, buffer, cursor, loc);
				module.buffer.erase(module.buffer.begin() + start_, module.buffer.begin() + cursor);
			}

			else if (keyword == "directive") {
				// TODO
			}

			else if (keyword == "macro") {
				// TODO
			}

			else if (keyword == "ifdef") {
				// TODO
			}
		}

		cursor++;
		loc.column++;
	}

	return module;
}

/* */
void Preprocessor::combine_modules(std::vector<uint8_t> &output, const std::unordered_map<std::string, PreprocessedModule> &processed_modules) {
	for (const std::string &module : preprocessor_context.graph->get_order()) {
		auto it = processed_modules.find(module);
		output.insert(output.end(), it->second.buffer.begin(), it->second.buffer.end());
	}
}