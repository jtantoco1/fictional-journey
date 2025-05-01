#include "catch_amalgamated.hpp"
#include "wordlist.h"

TEST_CASE("Fischer-Wagner Algorithm") {
    auto [str_a, str_b, ans_ref] = GENERATE(table <std::string, std::string, int>({
        {"sitting", "kitten", 3},
        {"saturday", "sunday", 3},
        {"acat", "acar", 1},
        {"a-cat", "an-act", 3},
        {"sitting", "", 7},
        {"", "sunday", 6},
    }));

    auto sdist = SmartMessage::StringMetric::distance_lev(str_a, str_b);

    CAPTURE(str_a, str_b, sdist, ans_ref);
    INFO("[Levenshtein] computed: " << sdist << " expected: " << ans_ref);
    REQUIRE(sdist == ans_ref);
}

TEST_CASE("Damerau-Levenshtein Algorithm") {
    auto [str_a, str_b, ans_ref] = GENERATE(table <std::string, std::string, int>({
        {"sitting", "kitten", 2},
        {"saturday", "sunday", 3},
        {"acat", "acar", 1},
        {"a-cat", "an-act", 2},
        {"sitting", "", 7},
        {"", "sunday", 6},
    }));

    auto sdist = SmartMessage::StringMetric::distance_dam_lev(str_a, str_b);

    CAPTURE(str_a, str_b, sdist, ans_ref);
    INFO("[DL] computed: " << sdist << " expected: " << ans_ref);
    REQUIRE(sdist == ans_ref);
}