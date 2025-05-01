#include <queue>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <set>

#include "wordlist.h"

namespace SmartMessage {
    void BKTree::add(const std::string& word) {
        // Normalize the word before adding
        std::string normalized_word = WordParser::normalize_word(word);
        if (normalized_word.empty()) {
            return;
        }

        // Add to words vector for easier iteration
        words.push_back(normalized_word);

        // If the tree is empty, create a root node
        if (!root) {
            root = std::make_unique<Node>();
            root->word = normalized_word;
            return;
        }

        // Find the right place to insert the word
        Node* current = root.get();
        while (true) {
            int distance = StringMetric::distance_dam_lev(current->word, normalized_word);
            
            // If word already exists (distance = 0), don't add it again
            if (distance == 0) {
                return;
            }

            // If there is no child at this distance, create one
            auto it = current->children.find(distance);
            if (it == current->children.end()) {
                auto new_node = std::make_unique<Node>();
                new_node->word = normalized_word;
                current->children[distance] = std::move(new_node);
                return;
            }

            // Move to the child node
            current = it->second.get();
        }
    }

    std::vector<std::pair<std::string, int>> BKTree::find_closest(const std::string& word, int max_dist) {
        std::vector<std::pair<std::string, int>> result;
        if (!root || max_dist < 0) {
            return result;
        }

        // Normalize the input word
        std::string normalized_word = WordParser::normalize_word(word);
        if (normalized_word.empty()) {
            return result;
        }

        // Use a queue for breadth-first search
        std::queue<Node*> queue;
        queue.push(root.get());

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            // Calculate distance between current node's word and the input word
            int distance = StringMetric::distance_dam_lev(current->word, normalized_word);
            
            // If distance is within the threshold, add to results
            if (distance <= max_dist) {
                result.push_back({current->word, distance});
            }

            // Determine valid distance range for children
            int lower_bound = distance - max_dist;
            int upper_bound = distance + max_dist;

            // Add relevant children to the queue
            for (int d = std::max(1, lower_bound); d <= upper_bound; ++d) {
                auto it = current->children.find(d);
                if (it != current->children.end()) {
                    queue.push(it->second.get());
                }
            }
        }

        // Sort results by distance
        std::sort(result.begin(), result.end(), 
            [](const auto& a, const auto& b) { return a.second < b.second; });

        // Limit to top 5 results
        if (result.size() > 5) {
            result.resize(5);
        }

        return result;
    }

    std::vector<std::string> BKTree::find_partial(const std::string& prefix) {
        // Not the most efficient way, but it works for BK-tree
        std::vector<std::string> result;
        
        // Normalize the prefix
        std::string normalized_prefix = WordParser::normalize_word(prefix);
        if (normalized_prefix.empty()) {
            return result;
        }

        // Search through all words and find those with the given prefix
        for (const auto& word : words) {
            if (word.substr(0, normalized_prefix.size()) == normalized_prefix) {
                result.push_back(word);
            }
        }

        // Sort and limit to top 5 results
        std::sort(result.begin(), result.end());
        if (result.size() > 5) {
            result.resize(5);
        }

        return result;
    }

    void BKTree::load_dictionary(const std::string& filename) {
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

    void BKTree::save_dictionary(const std::string& filename) {
        // Create directories if they don't exist
        std::filesystem::path file_path(filename);
        std::filesystem::create_directories(file_path.parent_path());

        // Open the file
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        // Sort words alphabetically
        std::sort(words.begin(), words.end());

        // Write words to file
        for (const auto& word : words) {
            file << word << std::endl;
        }

        file.close();
    }

    void BKTree::clear() {
        root.reset();
        words.clear();
    }
}