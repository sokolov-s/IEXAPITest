#include "config.h"
#include "common_utils/filesystem.h"
#include "common_utils/common_functions.h"
#include <exception>
#include <sstream>
#include <regex>

using namespace config;
namespace pt = boost::property_tree;
using namespace std;

Config::Config(const string &folder, const string &name)
    : configPath(folder + "/" + name)
    , configFolder(folder)
{
    Init();
}

int Config::GetInt(const string &key, const int &defValue)
{
    return Get<int>(key, defValue);
}

void Config::WriteInt(const string &key, const int value)
{
    Write(key, value);
}

string Config::GetString(const string &key, const string &defValue)
{
    lock_guard<mutex> locker(mtx);
    string res = defValue;
    try {
        res = root.get<string>(key, defValue);
    } catch(const pt::ptree_bad_path &) {
        WriteString(key, defValue);
    }

    return res;
}

void Config::WriteString(const string &key, const string &value)
{
    try {
	lock_guard<mutex> locker(mtx);
        root.put(key, value);
        pt::write_json(GetConfigPath(), root);
    } catch(const pt::ptree_bad_data &err) {
        std::string message = "Can't add (\"" + key + "\":\"" + value + "\" : " + err.what();
        throw std::runtime_error(message);
    } catch (const pt::json_parser_error &err) {
        throw std::runtime_error(string("Can't write json file : ") + err.what());
    }
}

string Config::GetFolder() const
{
    return configFolder;
}

std::vector<string> Config::GetRootObjects() const
{
    vector<string> res;
    for(const auto &it : root) {
        res.push_back(it.first);
    }
    return res;
}

void Config::Init()
{
    common_utils::filesystem::CreateFolder(GetFolder());
    struct stat buffer;
    if(stat(GetConfigPath().c_str(), &buffer) == 0) {
        try {
            pt::read_json(GetConfigPath(), root);
        } catch(...) {
            throw runtime_error("Can't parse config file : " + GetConfigPath());
        }
    }
}

string Config::ReplaceDots(string key, const std::string &replaceString)
{
    return common_utils::ReplaceString(key, "(\\.)", replaceString);
}

string Config::GetConfigPath() const
{
    return configPath;
}

ConfigManager &ConfigManager::GetInstance()
{
    static ConfigManager mngr;
    return mngr;
}

shared_ptr<Config> ConfigManager::GetConfig(const string &name)
{
    return GetConfig(kDefFolder, name);
}

std::shared_ptr<Config> ConfigManager::GetConfig(const string &folder, const string &name)
{
    lock_guard<mutex> locker(mtx);
    auto path = folder + "/" + name;
    auto it = configs.find(path);
    if(configs.end() == it) {
        auto cfg = make_shared<Config>(folder, name);
        configs.insert(make_pair(path, std::move(cfg)));
    }
    return configs.at(path);
}
