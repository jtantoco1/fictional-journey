#if !defined(WORDLIST_EEE121_H_)
#define WORDLIST_EEE121_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include "wordlist_base.h"

namespace SmartMessage {
    /**
     * BK-Tree implementation for spell checking
     * A BK-tree is a metric tree specifically adapted to discrete metric spaces
     * and is used for spell checking and approximate string matching.
     */
    class BKTree : public WordlistManager {
    public:
        BKTree() = default;
        virtual ~BKTree() = default;

        /**
         * Add a word to the BK-tree
         */
        void add(const std::string& word) override;

        /**
         * Find words with edit distance less than or equal to max_dist
         * Returns a vector of pairs (word, distance)
         */
        std::vector<std::pair<std::string, int>> find_closest(const std::string& word, int max_dist) override;

        /**
         * Find words with specified prefix
         * Not efficient for BK-tree, but implemented for compliance
         */
        std::vector<std::string> find_partial(const std::string& prefix) override;

        /**
         * Load words from file
         */
        void load_dictionary(const std::string& filename) override;

        /**
         * Save words to file
         */
        void save_dictionary(const std::string& filename) override;

        /**
         * Clear the tree
         */
        void clear() override;

    private:
        struct Node {
            std::string word;
            std::unordered_map<int, std::unique_ptr<Node>> children;
        };

        std::unique_ptr<Node> root;
        std::vector<std::string> words;
    };

    /**
     * Prefix Tree (Trie) implementation for autocompletion
     * A trie is a tree-like data structure used for storing a dynamic set or 
     * associative array where the keys are usually strings.
     */
    class PrefixTree : public WordlistManager {
    public:
        PrefixTree() = default;
        virtual ~PrefixTree() = default;

        /**
         * Add a word to the prefix tree
         */
        void add(const std::string& word) override;

        /**
         * Find closest words - not efficient for prefix tree but implemented for compliance
         */
        std::vector<std::pair<std::string, int>> find_closest(const std::string& word, int max_dist) override;

        /**
         * Find words with the specified prefix
         */
        std::vector<std::string> find_partial(const std::string& prefix) override;

        /**
         * Load words from file
         */
        void load_dictionary(const std::string& filename) override;

        /**
         * Save words to file
         */
        void save_dictionary(const std::string& filename) override;

        /**
         * Clear the tree
         */
        void clear() override;

    private:
        struct Node {
            bool is_end_of_word = false;
            std::unordered_map<char, std::unique_ptr<Node>> children;
        };

        std::unique_ptr<Node> root;

        // Helper function to collect words with given prefix
        void collect_words(Node* node, const std::string& prefix, std::vector<std::string>& result, int max_results = 5);
    };

    namespace StringMetric {
        /**
         * Compute the Levenshtein distance between two strings.
         * 
         * The Levenshtein distance measures the difference between the strings in terms of
         * insertion, deletion, and substitution from string `a` to string `b`.
         */
        int distance_lev(const std::string& a, const std::string& b);

        /**
         * Compute the Damerau-Levenshtein distance between two strings.
         * 
         * The Damerau-Levenshtein distance is a generalization on the Levenshtein distance
         * that factors in transposition of letters in computing the string difference. 
         */
        int distance_dam_lev(const std::string& a, const std::string& b);
    }
}

#endif