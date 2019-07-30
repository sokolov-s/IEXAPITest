#pragma once

#include <string>
#include <vector>
#include <fcntl.h>
#include <memory>

namespace common_utils {
namespace filesystem {

/**
 * @brief GetFileName - Function returns filename without folder part.
 */
std::string GetFileName(const std::string &fullPath);

/**
 * @brief GetDirectory - Function returns full directory path to the file without filename.
 */
std::string GetDirectory(const std::string &fullPath);

/**
 * @brief CreateFolder - Function recursively creates folders for log file.
 */

void CreateFolder(const std::string &dir);

bool IsFileExist(const std::string &filePath) noexcept;
void Remove(const std::string &filePath);

std::string GetMD5(const std::string &filePath);
size_t GetFileSize(const std::string &filePath);

} //namespace filesystem
} //namespace common_utils

