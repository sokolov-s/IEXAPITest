#pragma once

namespace common_utils {
namespace noncopyable {

struct NonCopyable
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable(const NonCopyable &&) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &&) = delete;
};

} //namespace noncopyable
} //namespace common_utils
