#if !defined(WORDLIST_BASE_EEE121_H_)
#define WORDLIST_BASE_EEE121_H_

#include <string>
#include <vector>
#include <fstream>

namespace SmartMessage {
    /**
     * Helper class for parsing words in a text
     */
    class WordParser {
    public:
        /**
         * Check if a character is a valid word character (a-z, A-Z, -, ')
         */
        static bool is_word_char(char c);

        /**
         * Trim whitespace from the beginning and end of a string
         */
        static std::string trim(const std::string& s);

        /**
         * Normalize a word by converting to lowercase and removing invalid characters
         */
        static std::string normalize_word(const std::string& word);
    };

    /**
     * Iterator for words in a text
     */
    class WordsIterator {
    public:
        /**
         * Constructor
         * @param text The text to iterate over
         */
        explicit WordsIterator(const std::string& text);

        /**
         * Iterator for words in a text
         */
        class iterator {
        public:
            /**
             * Constructor
             * @param pos Current position in the text
             * @param end End position of the text
             */
            iterator(std::string::const_iterator pos, std::string::const_iterator end);

            /**
             * Check if two iterators are equal
             */
            bool operator==(const iterator& other) const;

            /**
             * Check if two iterators are not equal
             */
            bool operator!=(const iterator& other) const;

            /**
             * Advance to the next word
             */
            iterator& operator++();

            /**
             * Get the current word
             */
            const std::string& operator*() const;

        private:
            std::string::const_iterator pos_;
            std::string::const_iterator end_;
            std::string current_word_;

            /**
             * Find the next word in the text
             */
            void find_next_word();
        };

        /**
         * Get iterator to the beginning of the text
         */
        iterator begin() const;

        /**
         * Get iterator to the end of the text
         */
        iterator end() const;

    private:
        std::string text_;
    };

    /**
     * Base class for wordlist managers
     */
    class WordlistManager {
    public:
        virtual ~WordlistManager() = default;

        /**
         * Add a word to the manager
         * @param word The word to add
         */
        virtual void add(const std::string& word) = 0;

        /**
         * Find words that are similar to the given word within a maximum distance
         * @param word The word to find similar words for
         * @param max_dist The maximum edit distance
         * @return Vector of pairs (word, distance)
         */
        virtual std::vector<std::pair<std::string, int>> find_closest(const std::string& word, int max_dist) = 0;

        /**
         * Find words with a given prefix
         * @param prefix The prefix to search for
         * @return Vector of words with the given prefix
         */
        virtual std::vector<std::string> find_partial(const std::string& prefix) = 0;

        /**
         * Find words similar to the given word with a similarity threshold
         * @param word The word to find similar words for
         * @param max_dist The maximum edit distance
         * @param threshold The minimum similarity threshold (0.0 - 1.0)
         * @return Vector of pairs (word, distance)
         */
        virtual std::vector<std::pair<std::string, int>> find_closest_similar(
            const std::string& word, int max_dist, double threshold);

        /**
         * Load a dictionary from a file
         * @param filename The filename to load from
         */
        virtual void load_dictionary(const std::string& filename) = 0;

        /**
         * Save a dictionary to a file
         * @param filename The filename to save to
         */
        virtual void save_dictionary(const std::string& filename) = 0;

        /**
         * Clear the manager
         */
        virtual void clear() = 0;

        /**
         * Load dictionaries from a directory
         * @param directory The directory to load from
         */
        virtual void load_dictionaries(const std::string& directory);

        /**
         * Save dictionaries to a directory
         * @param directory The directory to save to
         */
        virtual void save_dictionaries(const std::string& directory);

    protected:
        /**
         * Get a list of dictionary files in a directory
         * @param directory The directory to scan
         * @return Vector of filenames
         */
        std::vector<std::string> get_dictionary_files(const std::string& directory);
    };
}

#endif