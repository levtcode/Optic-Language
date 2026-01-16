/* trie_operators.cpp */

#include "trie_operators.hpp"

TrieNode *root = new TrieNode();

/* */
void TrieNode::insert(const std::string &op) {
    TrieNode *curr = root;

    for (char c : op) {
        if (curr->children[c] == nullptr) {
            TrieNode *new_node = new TrieNode();
            curr->children[c] = new_node;
        }

        curr = curr->children[c];
        
    }

    curr->is_final = true;
}

/* */
bool TrieNode::search(const std::string &op) {
    if (root == nullptr) {
        return false;
    }

    TrieNode *curr = root;
    for (char c : op) {
        if (curr->children[c] == nullptr) {
            return false;
        }

        curr = curr->children[c];
    }

    return true;
}

void init(const std::unordered_map<std::string_view, TokenType> &table) {
    for (auto it = table.begin(); it != table.end(); it++) {
        std::string op(it->first);
        root->insert(op);
    }
}