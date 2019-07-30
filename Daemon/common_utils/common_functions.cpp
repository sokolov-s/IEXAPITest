#include "common_functions.h"
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fstream>
#include <stdio.h>
#include <cstring>

using namespace std;

string common_utils::GenerateUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::ostringstream str;
    str << uuid;
    return str.str();
}

string common_utils::System(const string &command)
{
    FILE *fp = popen(command.c_str(), "r");
    char *system_res = nullptr;
    size_t len = 0;
    std::ostringstream str;
    if (fp != nullptr) {
        while(getline(&system_res, &len, fp) != -1) {
            fputs(system_res, stdout);
            str << system_res;
        }
    } else {
        throw std::runtime_error("Can't execute command (\"" + command + "\") : " + std::strerror(errno));
    }
    int status = pclose(fp);
    free(system_res);
    if(status != 0 ) {
        throw std::runtime_error("Command (\"" + command + "\") with error : " + to_string(status));
    }
    return str.str();
}

std::string common_utils::ReplaceString(const std::string &str, const std::string &expr, const std::string &replaceString)
{
    std::regex ee(expr);
    return std::regex_replace(str, ee, replaceString);
}
