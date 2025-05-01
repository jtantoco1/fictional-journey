#if !defined(WORDLIST_EEE121_H_)
#define WORDLIST_EEE121_H_

#include <string>
#include <vector>

#include "wordlist_base.h"

namespace SmartMessage {
    /**
     * TODO: Create data structures that extend WordlistManager
     * HINTS:
     * - The reference solution created two data structures that extend WordlistManager.
     *   - A prefix tree for autocompletion
     *     - Tricky to implement and inefficient memory-wise.
     *     - A compressed or Patricia tree is the recommended implementation.
     *   - A BK-tree for spellchecking
     *     - Very simple to implement
     * - It may help to implement the spellchecker first as it can be done using easily
     *   implementable, but not necessarily efficient, algorithms (i.e. sorting and iterating).
     * - Note that you are not limited to implementing the previously mentioned data structures.
     *   You can implement fewer or more than that as long as they extend WordlistManager.
     * - You are also not obliged to implement all of the `virtual` methods inside WordlistManager.
     *   But you need to have the `add()` and `find_partial()` methods.
     */

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