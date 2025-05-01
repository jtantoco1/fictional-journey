#include <queue>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <set>

#include "wordlist.h"

namespace SmartMessage {
    void PrefixTree::add(const std::string& word) {
        // Normalize the word before adding
        std::string normalized_word = WordParser::normalize_word(word);
        if (normalized_word.empty()) {
            return;
        }

        // Create root if it doesn't exist
        if (!root) {
            root = std::make_unique<Node>();
        }

        // Start from the root
        Node* current = root.get();

        // Traverse the tree, creating new nodes as needed
        for (char c : normalized_word) {
            // Create the child node if it doesn't exist
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = std::make_unique<Node>();
            }
            
            // Move to the child node
            current = current->children[c].get();
        }

        // Mark the end of the word
        current->is_end_of_word = true;
    }

    std::vector<std::pair<std::string, int>> PrefixTree::find_closest(const std::string& word, int max_dist) {
        // Not efficient for Trie, but implemented for compliance
        // Collect all words in the trie and then find the closest ones
        
        std::vector<std::string> all_words;
        collect_words(root.get(), "", all_words, -1);  // -1 to collect all words
        
        std::vector<std::pair<std::string, int>> result;
        std::string normalized_word = WordParser::normalize_word(word);
        
        for (const auto& w : all_words) {
            int distance = StringMetric::distance_dam_lev(w, normalized_word);
            if (distance <= max_dist) {
                result.push_back({w, distance});
            }
        }
        
        // Sort by distance
        std::sort(result.begin(), result.end(), 
            [](const auto& a, const auto& b) { return a.second < b.second; });
        
        // Limit to top 5 results
        if (result.size() > 5) {
            result.resize(5);
        }
        
        return result;
    }

    std::vector<std::string> PrefixTree::find_partial(const std::string& prefix) {
        std::vector<std::string> result;
        
        // Normalize the prefix
        std::string normalized_prefix = WordParser::normalize_word(prefix);
        if (normalized_prefix.empty() || !root) {
            return result;
        }
        
        // Start from the root
        Node* current = root.get();
        
        // Traverse to the node representing the prefix
        for (char c : normalized_prefix) {
            if (current->children.find(c) == current->children.end()) {
                // Prefix not found
                return result;
            }
            current = current->children[c].get();
        }
        
        // Collect all words with this prefix
        collect_words(current, normalized_prefix, result);
        
        return result;
    }

    void PrefixTree::collect_words(Node* node, const std::string& prefix, std::vector<std::string>& result, int max_results) {
        if (!node) {
            return;
        }
        
        // If this node marks the end of a word, add it to results
        if (node->is_end_of_word) {
            result.push_back(prefix);
            // Return if we've reached the maximum number of results
            if (max_results > 0 && result.size() >= static_cast<size_t>(max_results)) {
                return;
            }
        }
        
        // Recursively collect words from all children
        for (const auto& [c, child] : node->children) {
            collect_words(child.get(), prefix + c, result, max_results);
            // Stop if we've reached the maximum
            if (max_results > 0 && result.size() >= static_cast<size_t>(max_results)) {
                return;
            }
        }
    }

    void PrefixTree::load_dictionary(const std::string& filename) {
        // Clear existing tree
        clear();
        
        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        // Read words line by line
        std::string word;
        while (std::getline(file, word)) {
            // Add each word to the tree
            add(word);
        }
        
        file.close();
    }

    void PrefixTree::save_dictionary(const std::string& filename) {
        // Create directories if they don't exist
        std::filesystem::path file_path(filename);
        std::filesystem::create_directories(file_path.parent_path());
        
        // Open the file
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        // Collect all words in the trie
        std::vector<std::string> all_words;
        collect_words(root.get(), "", all_words, -1);  // -1 to collect all words
        
        // Sort words alphabetically
        std::sort(all_words.begin(), all_words.end());
        
        // Write words to file
        for (const auto& word : all_words) {
            file << word << std::endl;
        }
        
        file.close();
    }

    void PrefixTree::clear() {
        root.reset();
        root = std::make_unique<Node>();
    }
}