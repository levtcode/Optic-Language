/* preprocessor.cpp */

#include "preprocessor.hpp"
#include "diagnostic_engine.hpp"

#include <cctype>
#include <iostream>
#include <cstddef>
#include <string_view>

std::vector<Macro> macros;
std::vector<std::string> constant_expressions;

// ========== Dependency Graph methods ==========

#ifdef OPTIC_DEBUG

void DependencyGraph::print_nodes() {
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

GraphNode& DependencyGraph::add_node(const std::string &module) {
	auto it = nodes.try_emplace(module, module);
	return it.first->second;
}

void DependencyGraph::add_dependency(const std::string &module, const std::string &dependency) noexcept {
	auto &node = add_node(module);
	node.dependencies.push_back(dependency);
	add_node(dependency);
}

bool DependencyGraph::dfs(const GraphNode &node, std::unordered_map<std::string, VisitState> &visit) const noexcept {
	auto it = visit.find(node.module.module_name);
	
	if (it != visit.end()) {
		if (it->second == Visited)
			return false;
		if (it->second == Visiting)
			return true;
	}

	visit[node.module.module_name] = Visiting;

	for (auto &dep : node.dependencies) {
		auto dep_it = nodes.find(dep);
        if (dep_it != nodes.end()) {
            if (dfs(dep_it->second, visit)) return true;
        }
	}

	visit[node.module.module_name] = Visited;
	return false;
}

bool DependencyGraph::has_cycles() const noexcept {
	std::unordered_map<std::string, VisitState> visit;

	for (auto &[_, node] : nodes) {
		if (dfs(node, visit)) {
			return true;
		}
	}

	return false;
}

// ========== Preprocessor methods ==========

std::string Preprocessor::get_module_name(const std::string_view &buffer, size_t &cursor) {
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

std::string Preprocessor::read_keyword(const std::string_view &buffer, size_t &cursor) {
	std::string keyword;

	while (cursor < buffer.size() && isalpha(buffer[cursor])) {
		keyword += buffer[cursor++];
	}

	return keyword;
}

void Preprocessor::analyze(std::string_view buffer) {
	size_t cursor = 0;

	while (cursor < buffer.size()) {
		if (buffer[cursor] == '/') {
			cursor++;

			if (buffer[cursor] == '/') {
				while (cursor < buffer.size() && buffer[cursor] != '\n') cursor++;
			} else if (buffer[cursor] == '*') {
				cursor++;
				
				while (cursor < buffer.size() - 1 && !(buffer[cursor] == '*' && buffer[cursor + 1] == '/')) {
					if (buffer[cursor] == '\n') {
						loc.line++;
						loc.column = 1;
					}
					cursor++;
				}
			}

			continue;
		}

		if (buffer[cursor] == '\n') {
			loc.line++;
			loc.column = 1;
			cursor++;
			continue;
		}

		if (isalpha(buffer[cursor])) {
			std::string keyword = read_keyword(buffer, cursor);

			if (keyword == "import") {
				while (cursor < buffer.size() && isspace(buffer[cursor]) && buffer[cursor] != '\n') cursor++;

				std::string dep_name = get_module_name(buffer, cursor);
				graph.add_dependency(module.module_name, dep_name);
			}

			// if (...) More statements
		}

		cursor++;
		loc.column++;
	}
}