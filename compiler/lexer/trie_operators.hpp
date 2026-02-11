/* trie_operators.hpp */

#pragma once

#include "tokens.hpp"

#include <unordered_map>
#include <string>

struct TrieNode {
    bool is_final = false;
    std::unordered_map<char, TrieNode*> children;

    ~TrieNode() {
        for (auto& [_, node] : children) {
            delete node;
        }
    }

    void insert(const std::string&);
    bool search(const std::string&);
};

extern TrieNode *root;
void init(const std::unordered_map<std::string_view, TokenType>&);