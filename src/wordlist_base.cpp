#include <cctype>
#include <algorithm>
#include <string>
#include <filesystem>

#include "wordlist_base.h"

namespace SmartMessage {
    bool WordParser::is_word_char(char c) {
        // Check if character is a letter, hyphen, or apostrophe
        return std::isalpha(c) || c == '-' || c == '\'';
    }

    std::string WordParser::trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            ++start;
        }

        auto end = s.end();
        if (start != s.end()) {
            --end;
            while (end > start && std::isspace(*end)) {
                --end;
            }
            ++end;
        }

        return std::string(start, end);
    }

    std::string WordParser::normalize_word(const std::string& word) {
        std::string normalized;
        normalized.reserve(word.size());

        // Convert all characters to lowercase and keep only valid word characters
        for (char c : word) {
            if (is_word_char(c)) {
                normalized.push_back(std::tolower(c));
            }
        }

        return normalized;
    }

    // WordsIterator implementation
    WordsIterator::WordsIterator(const std::string& text) : text_(text) {}

    WordsIterator::iterator WordsIterator::begin() const {
        return iterator(text_.begin(), text_.end());
    }

    WordsIterator::iterator WordsIterator::end() const {
        return iterator(text_.end(), text_.end());
    }

    // WordsIterator::iterator implementation
    WordsIterator::iterator::iterator(std::string::const_iterator pos, std::string::const_iterator end)
        : pos_(pos), end_(end) {
        // Find the first word
        find_next_word();
    }

    void WordsIterator::iterator::find_next_word() {
        current_word_.clear();

        // Skip non-word characters
        while (pos_ != end_ && !WordParser::is_word_char(*pos_)) {
            ++pos_;
        }

        // Build the word
        while (pos_ != end_ && WordParser::is_word_char(*pos_)) {
            current_word_.push_back(*pos_);
            ++pos_;
        }
    }

    bool WordsIterator::iterator::operator==(const iterator& other) const {
        return pos_ == other.pos_ && current_word_ == other.current_word_;
    }

    bool WordsIterator::iterator::operator!=(const iterator& other) const {
        return !(*this == other);
    }

    WordsIterator::iterator& WordsIterator::iterator::operator++() {
        find_next_word();
        return *this;
    }

    const std::string& WordsIterator::iterator::operator*() const {
        return current_word_;
    }

    // WordlistManager implementation
    std::vector<std::pair<std::string, int>> WordlistManager::find_closest_similar(
        const std::string& word, int max_dist, double threshold) {
        // Default implementation - can be overridden by derived classes
        auto result = find_closest(word, max_dist);
        
        // Filter based on threshold
        auto it = std::remove_if(result.begin(), result.end(),
            [threshold, word](const auto& item) {
                double similarity = 1.0 - static_cast<double>(item.second) / 
                    std::max(word.length(), item.first.length());
                return similarity < threshold;
            });
        
        result.erase(it, result.end());
        return result;
    }

    std::vector<std::string> WordlistManager::get_dictionary_files(const std::string& directory) {
        std::vector<std::string> files;
        
        try {
            // Check if directory exists
            if (!std::filesystem::exists(directory)) {
                return files;
            }
            
            // Iterate through directory entries
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".smwdl") {
                    files.push_back(entry.path().string());
                }
            }
        } catch (const std::filesystem::filesystem_error&) {
            // Ignore filesystem errors
        }
        
        // Sort files by name
        std::sort(files.begin(), files.end());
        return files;
    }

    // Default load/save operations for managers
    void WordlistManager::load_dictionaries(const std::string& directory) {
        auto files = get_dictionary_files(directory);
        for (const auto& file : files) {
            load_dictionary(file);
        }
    }

    void WordlistManager::save_dictionaries(const std::string& directory) {
        // Default implementation - create separate files for each letter
        std::unordered_map<char, std::vector<std::string>> words_by_letter;
        
        // Get all words and group by first letter
        for (char letter = 'a'; letter <= 'z'; ++letter) {
            // Find words starting with this letter
            std::string prefix(1, letter);
            auto words = find_partial(prefix);
            
            if (!words.empty()) {
                words_by_letter[letter] = std::move(words);
            }
        }
        
        // Create the directory if it doesn't exist
        std::filesystem::create_directories(directory);
        
        // Save each letter group to a separate file
        for (const auto& [letter, words] : words_by_letter) {
            std::string filename = directory + "/" + letter + ".smwdl";
            std::ofstream file(filename);
            
            if (file.is_open()) {
                for (const auto& word : words) {
                    file << word << std::endl;
                }
                file.close();
            }
        }
    }
}