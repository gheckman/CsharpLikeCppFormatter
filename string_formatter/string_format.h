/*
@file string_format.h
@brief Formats strings.
*/

#pragma once

#include <algorithm>
#include <sstream>
#include <string>

/*
Formats strings.
*/
namespace string_format
{
    /*
    Detail namespace.
    */
    namespace detail
    {
        /*
        Converts the type T to a string.
        Only exists if the type T is a valid argument to std::to_string.
        @tparam T Some type
        @param [in] t  Some instance of type T
        @param [in] _2 Bool tag
        */
        template <typename T>
        auto get_t_as_string_tagged_(T&& t, bool) -> decltype(std::to_string(t))
        {
            return std::to_string(t);
        }

        /*
        Converts the type T to a string using a stringstream.
        @tparam T Some type
        @param [in] t  Some instance of type T
        @param [in] _2 Int tag
        */
        template <typename T>
        std::string get_t_as_string_tagged_(T&& t, int)
        {
            std::stringstream ss;
            ss << t;
            return ss.str();
        }

        /*
        Converts the type T to a string.
        @tparam T Some type
        @param [in] t Some instance of type T
        */
        template <typename T>
        auto get_t_as_string_(T&& t)
        {
            // Uses tag dispatch to prioritize the more efficent string conversion
            return get_t_as_string_tagged_(std::forward<T>(t), true);
        }

        /*
        Converts the type T to a string.
        @tparam T A character type
        @param [in] t Some instance of type T
        */
        template <>
        auto get_t_as_string_<char>(char&& t)
        {
            return std::string(1, t);
        }

        /*
        Formats text based on bracket delimited indecies and arguments.
        This is the base case of the recursive parameter pack call.
        Increments the index of the format string.
        @tparam T Some type
        @param [in, out] s Format string
        @param [in, out] i Index of bracketed format text to replace
        @param [in]      t Some instance of type T
        */
        template <typename T>
        void format_detail_(std::string& s, int& i, T&& t)
        {
            std::string t_to_string(get_t_as_string_(std::forward<T>(t)));
            std::string format_index = '{' + std::to_string(i) + '}';
            auto pos = s.find(format_index);
            while (pos != std::string::npos)
            {
                s.replace(pos, format_index.size(), t_to_string);
                pos = s.find(format_index, pos + 1);
            }
            ++i;
        }

        /*
        Formats text based on bracket delimited indecies and arguments.
        This is the base case of the recursive parameter pack call.
        @tparam T Some type
        @param [in, out] s Format string
        @param [in]      i Index of bracketed format text to replace
        @param [in]      t Last object in parameter pack
        */
        template <typename T>
        void format_(std::string& s, int i, T&& t)
        {
            format_detail_(s, i, std::forward<T>(t));
        }

        /*
        Formats text based on bracket delimited indecies and arguments.
        Splits off the first object in the parameter pack to be formatted.
        Then calls itself recursively.
        @tparam T       Some type
        @tparam ...Args Variadic arguments
        @param [in, out] s       Format string
        @param [in]      i       Index of bracketed format text to replace
        @param [in]      t       First object in parameter pack
        @param [in]      ...args The rest of the parameter pack
        */
        template <typename T, typename ...Args>
        void format_(std::string& s, int i, T&& t, Args&&... args)
        {
            format_detail_(s, i, std::forward<T>(t));
            format_(s, i, std::forward<Args>(args)...);
        }
    }

    /*
    Just returns the string passed in.
    @param [in] s Format string
    */
    inline std::string format(std::string s) { return s; }

    /*
    Formats text based on bracket delimited indecies and arguments.
    Example: format("{1} bar {0}", 1.2345, "foo") will yield "foo bar 1.2345"
    @tparam ...Args Variadic arguments
    @param [in] s       Format string
    @param [in] ...args Parameter pack
    */
    template <typename ...Args>
    std::string format(std::string s, Args&&... args)
    {
        detail::format_(s, 0, std::forward<Args>(args)...);
        return s;
    }
}