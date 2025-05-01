#include <vector>
#include <string>
#include <iterator>

#include "catch_amalgamated.hpp"
#include "wordlist.h"

TEST_CASE("WordsIterator") {
    auto [in_str, out_vec] = GENERATE(table <std::string, std::vector <std::string>>({
        {"hello", {"hello"}},
        {"hello world", {"hello", "world"}},
        {"hello world!!!", {"hello", "world"}},
        {"this-is a-string", {"this-is", "a-string"}},
        {"lorem ipsum dolor sit amet consectetur adisipicing elit nonummy", {"lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adisipicing", "elit", "nonummy"}},
    }));

    auto words_it = SmartMessage::WordsIterator(in_str);
    int words_it_len {};

    for(auto [wit, rit] = std::make_tuple(words_it.begin(), out_vec.begin()); wit != words_it.end() && rit != out_vec.end(); ++wit, ++rit) {
        words_it_len += 1;

        CAPTURE(*wit, *rit);
        CHECK(*wit == *rit);
    }

    INFO("[WordsIterator] Word len output: " << words_it_len << " expected: " << out_vec.size());
    REQUIRE(words_it_len == out_vec.size());
}

TEST_CASE("Word parser is word char") {
    auto [in_char, out_bool] = GENERATE(table <char, bool>({
        {'A', true},
        {'d', true},
        {'\'', true},
        {'\n', false},
        {'$', false},
    }));

    auto ans_bool = SmartMessage::WordParser::is_word_char(in_char);

    CAPTURE(in_char, out_bool, ans_bool);
    REQUIRE(ans_bool == out_bool);
}

TEST_CASE("Word parser trim") {
    auto [in_str, out_str] = GENERATE(table <std::string, std::string>({
        {"hello", "hello"},
        {"     hello     ", "hello"},
        {"hello   \n", "hello"},
        {"\n     \nhello", "hello"},
        {"   hello world!   ", "hello world!"},
    }));

    auto ans_str = SmartMessage::WordParser::trim(in_str);

    CAPTURE(in_str, out_str, ans_str);
    REQUIRE(ans_str == out_str);
}