#pragma once

#include <common_utils/noncopyable.h>
#include <string>
#include <map>
#include <tuple>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <ostream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace config {

namespace additions {

class Section {
public:
    Section(const std::string & sectionName) : name(sectionName){}
    Section(std::string && sectionName) noexcept : name(std::move(sectionName)){}

    Section(Section &&rhs) noexcept
    {
        *this = std::move(rhs);
    }

    Section(const Section &rhs)
        : name(rhs.name)
    {}

    Section & operator=(const Section &) = default;
    Section & operator=(Section &&) = default;

    std::string operator()(const std::string & key) const noexcept {return name + "." + key;}
    std::string operator()() const noexcept {return name;}

    bool operator==(const Section &rhs) const noexcept {
        return rhs.name == name;
    }

    bool operator<(const Section &rhs) const noexcept {
        return name < rhs.name;
    }

    friend std::ostream & operator<<(std::ostream &s, const Section &section) {
        s << section.name;
        return s;
    }

private:
    std::string name;
};

template<typename T>
struct Key {
    Key(){}
    Key(const Section &sec, const T &k, const std::string &name)
        : section(sec)
        , key(k)
        , key_name(name)
    {}

    Key(const Key &)  = default;
    template<typename K> Key(const Key &rhs) noexcept
        : section(rhs.section)
        , key(rhs.key)
        , key_name(rhs.key_name)
    {}

    Key(Key &&) noexcept = default;
    template<typename K> Key(const Key &&rhs) noexcept {*this = std::move(rhs);}

    Key & operator=(const Key &) = default;
    Key & operator=(const Key &&rhs) noexcept {*this = std::move(rhs);}
    template<typename K> Key & operator=(const Key &&rhs) {*this = std::move(rhs);}
    bool operator==(const Key &rhs) const {
        return (section == rhs.section) && (key == rhs.key) && (key_name == rhs.key_name);
    }
    Section section;
    T key;
    std::string key_name;
};

template<typename T>
class KeyManager {
public:
    KeyManager() noexcept {}

    KeyManager(std::initializer_list<Key<T>> & initKeys) noexcept
        :keys(initKeys)
    {}

    KeyManager(std::initializer_list<Key<T>> && initKeys) noexcept
        : keys(std::move(initKeys))
    {
    }

    KeyManager & operator=(const KeyManager & rhs) noexcept
    {
        std::copy(rhs.keys.begin(), rhs.keys.end(), keys);
    }

    KeyManager & operator=(KeyManager && rhs) noexcept
    {
        *this = std::move(rhs);
    }

    std::string operator()(const Section &section, const T & key) const
    {
        for(const auto &item : keys) {
            if(section == item.section && key == item.key) {
                return item.section(item.key_name);
            }
        }
        std::string msg;
        msg = "Key " + std::to_string(static_cast<int>(key)) + " does not found in "
                + section() + " section config keys";
        throw std::invalid_argument(msg);
    }

    void AddKey(Key<T> key) {
        for(const auto &item: keys) {
            if(item == key)
                return;
        }
        keys.push_back(std::move(key));
    }
private:
    std::vector<Key<T>> keys;
};

} //namespace additions

class Config : private common_utils::noncopyable::NonCopyable
{
public:
    Config(const std::string &folder, const std::string &name);

    int GetInt(const std::string &key, const int &defValue = 0);
    template<typename T>
    T Get(const std::string &key, const T &defValue = T()) {
        return boost::lexical_cast<T>(GetString(key, boost::lexical_cast<std::string>(defValue)));
    }

    template<typename T>
    std::vector<T> GetArray(const std::string &key, const std::vector<T> &defValue = {}) {
        std::lock_guard<std::mutex> locker(mtx);
        std::vector<T> res;
        try {
            for(const auto &item : root.get_child(key)) {
                res.push_back(item.second.data());
            }
            return res.empty() ? defValue : res;
        } catch(const boost::property_tree::ptree_bad_path &) {
            Write(key, defValue);
            return defValue;
        }
    }

    void WriteInt(const std::string &key, const int value);
    template<typename T>
    void Write(const std::string &key, const T value) {
        WriteString(key, boost::lexical_cast<std::string>(value));
    }

    template<typename T>
    void Write(const std::string &key, const std::vector<T> values) {
        try {
            std::lock_guard<std::mutex> locker(mtx);
            boost::property_tree::ptree child;
            for(const auto &value : values) {
                boost::property_tree::ptree obj;
                obj.put("", value);
                child.push_back(std::make_pair("", obj));
            }
            if(root.find(key) == root.not_found()) {
                root.add_child(key, child);
            }
            boost::property_tree::write_json(GetConfigPath(), root);
        } catch(const boost::property_tree::ptree_bad_data &err) {
            std::string message = "Can't add (\"" + key + "\": array_of_values " + err.what();
            throw std::runtime_error(message);
        } catch (const boost::property_tree::json_parser_error &err) {
            throw std::runtime_error(std::string("Can't write json file : ") + err.what());
        }
    }

    std::string GetString(const std::string &key, const std::string &defValue = "");
    void WriteString(const std::string &key, const std::string &value);

    std::string GetFolder() const;

    std::vector<std::string> GetRootObjects() const;

    std::string GetConfigPath() const;

    std::string ReplaceDots(std::string key, const std::string &replaceString);
private:
    void Init();
private:
    boost::property_tree::ptree root;
    std::mutex mtx;
    std::string configPath;
    std::string configFolder;
};

class ConfigManager : private common_utils::noncopyable::NonCopyable
{
    ConfigManager(){}
public:
    static ConfigManager & GetInstance();
    std::shared_ptr<Config> GetConfig(const std::string &name);
    std::shared_ptr<Config> GetConfig(const std::string &folder, const std::string &name);

private:
    const std::string kDefFolder = ".";
    std::map<std::string, std::shared_ptr<Config>> configs;
    std::mutex mtx;
};

} //namespace config
