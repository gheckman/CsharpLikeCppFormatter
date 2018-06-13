/*
@file string_format.h
@brief Formats strings.
*/

#pragma once

#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

#include "regex_ext.h"

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
        Struct used for tag dispatching.
        @tparam T Some type
        */
        template <typename T>
        struct class_tag {};

        /*
        Converts the type T to a string.
        Only exists if the type T is a valid argument to std::to_string.
        @tparam T Some type
        @param [in] t  Some instance of type T
        @param [in] _2 Bool tag
        */
        template <typename T>
        inline auto get_t_as_string_tagged_(const T& t, bool) -> decltype(std::to_string(t))
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
        inline std::string get_t_as_string_tagged_(const T& t, int)
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
        inline std::string get_t_as_string_(const T& t)
        {
            // Uses a simple bool/int based tag dispatch to prioritize the more efficent string conversion
            return get_t_as_string_tagged_(t, true);
        }

        /*
        Converts the type char to a string.
        @tparam T A character type
        @param [in] t Some instance of type char
        */
        template <>
        inline std::string get_t_as_string_<char>(const char& t)
        {
            return std::string(1, t);
        }

        /*
        Converts the type T to a string.
        @tparam T Some type
        @param [in] t Some instance of type T
        */
        template <typename T, typename ClassTag>
        inline std::string get_t_as_string_impl_(const T& t, const std::string& argument, ClassTag)
        {
            // Selects the basic formatter with no arguments
            return get_t_as_string_(t);
        }

        /*
        Converts the type T to a string.
        This is the entry point to the more specific formatting functions.
        @tparam T Some type
        @param [in] t        Some instance of type T
        @param [in] argument Argument to the formatter. Argument format is dependant on type T
        */
        template <typename T>
        inline std::string get_t_as_string_(const T& t, const std::string& argument)
        {
            // Uses tag dispatch to select the appropriate parameterized formatter based on the type
            class_tag<std::decay_t<T>> tag{};
            return get_t_as_string_impl_(t, argument, tag);
        }

#pragma region floating point

        /*
        Tagged function for choosing the floating point formatter for float.
        @tparam T float
        @param [in] t        Some instance of float
        @param [in] argument Can be of the form Fa, Ea, Xa, where a is some integer
        @param [in] _3       Tag for float
        */
        template <typename T>
        inline std::string get_t_as_string_impl_(const T& t, const std::string& argument, class_tag<float>)
        {
            return get_floating_point_as_string_(t, argument);
        }

        /*
        Tagged function for choosing the floating point formatter for double.
        @tparam T double
        @param [in] t        Some instance of double
        @param [in] argument Can be of the form Fa, Ea, Xa, where a is some integer
        @param [in] _3       Tag for double
        */
        template <typename T>
        inline std::string get_t_as_string_impl_(const T& t, const std::string& argument, class_tag<double>)
        {
            return get_floating_point_as_string_(t, argument);
        }

        /*
        Tagged function for choosing the floating point formatter for long double.
        @tparam T long double
        @param [in] t        Some instance of long double
        @param [in] argument Can be of the form Fa, Ea, Xa, where a is some integer
        @param [in] _3       Tag for long double
        */
        template <typename T>
        inline std::string get_t_as_string_impl_(const T& t, const std::string& argument, class_tag<long double>)
        {
            return get_floating_point_as_string_(t, argument);
        }

        /*
        Parameterized floating point formatter.
        Formats float, double, or long double types.
        @tparam T  float, double, or long double
        @tparam _2 Disables the function is the type argument is not float, double, or long double
        @param [in] t        Some instance of type T
        @param [in] argument Can be of the form Fa, Ea, Xa, where a is some integer
        */
        template <typename T, typename = std::enable_if<std::is_floating_point<T>::value>>
        inline std::string get_floating_point_as_string_(const T& t, const std::string& argument)
        {
            // Floating point (float, double, long double) formatter
            std::stringstream ss;
            switch (::tolower(argument[0]))
            {
                case 'f': ss << std::fixed; break;
                case 'e': ss << std::scientific; break;
                case 'x': ss << std::hexfloat; break;
            }
            if (argument.size() > 1)
                ss << std::setprecision(std::min(::atoi(&argument[1]), 99));
            ss << t;
            return ss.str();
        }

#pragma endregion

        /*
        Regex formatting callback.
        Formats a single regex match.
        @tparam T Some type
        @param [in] sm Sub-match expression. Example: {0} or {0:arg}.
        @param [in] t  Some instance of type T
        */
        template <typename T>
        inline std::string format_match_(const std::smatch& sm, const T& t)
        {
            auto argument = sm.str(1);
            return argument.empty() ? get_t_as_string_(t) : get_t_as_string_(t, argument.substr(1));
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
        inline void format_detail_(std::string& s, int& i, const T& t)
        {
            std::regex r("\\{" + std::to_string(i) + "(:[a-zA-Z0-9 -]*)?\\}");
            s = std_ext::regex_replace(s, r, [&](const std::smatch& sm) { return format_match_(sm, t); });
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
        inline void format_(std::string& s, int i, const T& t)
        {
            format_detail_(s, i, t);
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
        inline void format_(std::string& s, int i, const T& t, Args&&... args)
        {
            format_detail_(s, i, t);
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
    inline std::string format(std::string s, Args&&... args)
    {
        detail::format_(s, 0, std::forward<Args>(args)...);
        return s;
    }
}