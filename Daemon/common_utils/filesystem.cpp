#include "filesystem.h"
#include <ios>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace common_utils::filesystem;

std::string common_utils::filesystem::GetFileName(const std::string &fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    if(std::string::npos != pos) {
        return fullPath.substr(pos + 1);
    } else {
        return fullPath;
    }
}

std::string common_utils::filesystem::GetDirectory(const std::string &fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    if(std::string::npos != pos) {
        return fullPath.substr(0, pos);
    } else {
        return "./";
    }
}

void common_utils::filesystem::CreateFolder(const std::string &dir)
{
    std::string next_part(dir);
    std::string created_dir;
    struct stat buffer;
    if(dir.empty() || stat(dir.c_str(), &buffer) == 0) {
        return;
    }
    if(next_part[0] == '/') {
        created_dir += "/";
        next_part.erase(0, 1);
    }
    while (!next_part.empty()) {
        size_t pos = next_part.find("/");
        if (pos != std::string::npos) {
            created_dir += next_part.substr(0, pos);
            next_part.erase(0, pos + 1);
        } else {
            created_dir += next_part;
            next_part.clear();
        }
        if(!created_dir.empty() && stat(created_dir.c_str(), &buffer) != 0) {
            if (mkdir(created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                return;
            }
            chmod (created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
        created_dir += "/";
    }
}

bool common_utils::filesystem::IsFileExist(const std::string &filePath) noexcept
{
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

void common_utils::filesystem::Remove(const std::string &filePath)
{
    if(-1 == remove(filePath.c_str())) {
        throw std::ios_base::failure("Can't remove file: " + filePath);
    }
}

std::string common_utils::filesystem::GetMD5(const std::string &filePath)
{
    if(!IsFileExist(filePath)) {
        throw std::ios_base::failure("Can't find file: " + filePath);
    }
    MD5_CTX ctx;
    MD5_Init(&ctx);

    std::ifstream ifs(filePath, std::ios::binary);

    char file_buffer[4096];
    while (ifs.read(file_buffer, sizeof(file_buffer)) || ifs.gcount()) {
        MD5_Update(&ctx, file_buffer, ifs.gcount());
    }
    unsigned char digest[MD5_DIGEST_LENGTH] = {};
    MD5_Final(digest, &ctx);
    std::ostringstream md5sum;
    for(size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
        md5sum << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    return md5sum.str();
}

size_t common_utils::filesystem::GetFileSize(const std::string &filePath)
{
    struct stat status;
    if(stat(filePath.c_str(), &status) == -1) {
        throw std::ios_base::failure("Can't get stat of file: " + filePath);
    }
    return status.st_size;
}
