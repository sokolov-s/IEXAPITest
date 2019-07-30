#pragma once

#include <string>
#include <vector>
#include <regex>

namespace common_utils {

std::string GenerateUUID();

template<typename I>
std::vector<typename I::pointer> GetCStyleStringVector(const std::vector<I> &argv)
{
    std::vector<typename I::pointer> cstrings;
    cstrings.reserve(argv.size());
    for(const auto &item : argv)
        cstrings.push_back(const_cast<typename I::pointer>(item.c_str()));
    return std::move(cstrings);
}

std::string System(const std::string &command);

std::string ReplaceString(const std::string &str, const std::string &expr, const std::string &replaceString);

} //namespace common_utils
