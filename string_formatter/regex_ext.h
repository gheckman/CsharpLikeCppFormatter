#pragma once

#include <string>
#include <regex>

namespace std_ext
{
    template<class BidirIt, class Traits, class CharT, class UnaryFunction>
    std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
    {
        std::basic_string<CharT> s;

        typename std::match_results<BidirIt>::difference_type position_of_last_match = 0;
        auto end_of_last_match = first;

        auto callback = [&](const std::match_results<BidirIt>& match)
        {
            auto position_of_this_match = match.position(0);
            auto diff = position_of_this_match - position_of_last_match;

            auto start_of_this_match = end_of_last_match;
            std::advance(start_of_this_match, diff);

            s.append(end_of_last_match, start_of_this_match);
            s.append(f(match));

            auto length_of_match = match.length(0);

            position_of_last_match = position_of_this_match + length_of_match;

            end_of_last_match = start_of_this_match;
            std::advance(end_of_last_match, length_of_match);
        };

        std::regex_iterator<BidirIt> begin(first, last, re), end;
        std::for_each(begin, end, callback);

        s.append(end_of_last_match, last);

        return s;
    }

    template<class Traits, class CharT, class UnaryFunction>
    std::string regex_replace(const std::string& s, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
    {
        return regex_replace(s.cbegin(), s.cend(), re, f);
    }
}