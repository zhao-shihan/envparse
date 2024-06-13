#pragma once
#ifndef ENVPARSE_PARSE_de15b744524cffd6412cd5c6da54aff1e4433f8e6e8c29b63650bea9d4263a13
#define ENVPARSE_PARSE_de15b744524cffd6412cd5c6da54aff1e4433f8e6e8c29b63650bea9d4263a13

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

namespace envparse {

/// @brief The behavior when any environment variable do not exists.
enum struct not_set_option {
    exception,
    left_blank
};

namespace internal {
template<not_set_option Option>
auto parse_impl(std::string& str, std::size_t pos = 0) -> std::string&;
} // namespace internal

/// @brief Parse a string with environment variables (${...}) and replace them
/// with corresponding values. If the string does not contains ${...} then it is
/// returned as is.
/// @tparam Option controlling the behavior when any environment variable
/// do not exists. Can be `envparse::not_set_option::exception` (default) or
/// `envparse::not_set_option::left_blank`.
/// @param str A string with environment variables (${...}).
/// @return Parsed result.
/// @note If an environment variable is not set, the default behavior is to
/// throw an exception ("Exception"). The behavior can be changed to replace
/// them with blank by setting `option` to "Ignore". An environment variable
/// must be surrounded by "${}", $ENV syntax is not supported.
/// @exception std::runtime_error if anything unexcepted happens.
template<not_set_option Option = not_set_option::exception>
[[nodiscard]] auto parse(std::string str) -> std::string {
    return std::move(internal::parse_impl<Option>(str));
}

namespace internal {

template<not_set_option Option>
auto parse_impl(std::string& str, std::size_t pos) -> std::string& {
    const auto dollar{str.find_first_of('$', pos)};
    if (dollar == std::string::npos) {
        return str;
    }
    const auto l_brace{dollar + 1};
    if (str[l_brace] != '{') {
        throw std::runtime_error{"envparse::parse: Environment variable must "
                                 "be provided in the form of ${...}"};
    }
    const auto r_brace{str.find_first_of('}', dollar + 2)};
    if (r_brace == std::string::npos) {
        throw std::runtime_error{"envparse::parse: Brace not closed"};
    }
    const auto env_var{str.substr(l_brace + 1, r_brace - l_brace - 1)};
    const char* env{std::getenv(env_var.c_str())};
    if (env == nullptr) {
        switch (Option) {
        case not_set_option::exception:
            throw std::runtime_error{"envparse::parse: Environment variable '" +
                                     env_var + "' has not set or invalid"};
            break;
        case not_set_option::left_blank:
            env = "";
            break;
        }
    }
    str.replace(dollar, env_var.size() + 3, env);
    return parse_impl<Option>(str, dollar);
}

} // namespace internal

} // namespace envparse

#endif
