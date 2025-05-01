#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "wordlist.h"

namespace SmartMessage::StringMetric {
    int distance_lev(const std::string& a, const std::string& b) {
        // Implementation of Levenshtein distance algorithm
        const int a_length = a.length();
        const int b_length = b.length();
        
        // Initialize matrix of size (a_length+1) x (b_length+1)
        std::vector<std::vector<int>> distance(a_length + 1, std::vector<int>(b_length + 1, 0));
        
        // Initialize first column
        for (int i = 0; i <= a_length; i++) {
            distance[i][0] = i;
        }
        
        // Initialize first row
        for (int j = 0; j <= b_length; j++) {
            distance[0][j] = j;
        }
        
        // Fill the distance matrix
        for (int i = 1; i <= a_length; i++) {
            for (int j = 1; j <= b_length; j++) {
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                
                distance[i][j] = std::min({
                    distance[i - 1][j] + 1,         // deletion
                    distance[i][j - 1] + 1,         // insertion
                    distance[i - 1][j - 1] + cost   // substitution
                });
            }
        }
        
        return distance[a_length][b_length];
    }

    int distance_dam_lev(const std::string& a, const std::string& b) {
        // Implementation of Damerau-Levenshtein distance algorithm
        const int a_length = a.length();
        const int b_length = b.length();
        
        // Handle edge cases
        if (a_length == 0) return b_length;
        if (b_length == 0) return a_length;
        
        // Initialize matrix of size (a_length+1) x (b_length+1)
        std::vector<std::vector<int>> distance(a_length + 1, std::vector<int>(b_length + 1, 0));
        
        // Initialize first column and row
        for (int i = 0; i <= a_length; i++) {
            distance[i][0] = i;
        }
        
        for (int j = 0; j <= b_length; j++) {
            distance[0][j] = j;
        }
        
        // Fill the distance matrix
        for (int i = 1; i <= a_length; i++) {
            for (int j = 1; j <= b_length; j++) {
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                
                distance[i][j] = std::min({
                    distance[i - 1][j] + 1,         // deletion
                    distance[i][j - 1] + 1,         // insertion
                    distance[i - 1][j - 1] + cost   // substitution
                });
                
                // Check for transposition (swap of adjacent characters)
                if (i > 1 && j > 1 && a[i - 1] == b[j - 2] && a[i - 2] == b[j - 1]) {
                    distance[i][j] = std::min(distance[i][j], distance[i - 2][j - 2] + 1);
                }
            }
        }
        
        return distance[a_length][b_length];
    }
}