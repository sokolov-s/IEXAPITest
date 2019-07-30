#pragma once

#include "config.h"
#include "common_utils/noncopyable.h"

namespace config {

class Daemon : private common_utils::noncopyable::NonCopyable
{
    enum class eKeys {
        LOG_PATH,
        DB_PATH,
        INTERFACE,
        PORT,
        IEX_URL,
        IEX_TOCKEN,
        IEX_UPDATE_PRICE_SECOND,
        IEX_UPDATE_SYMBOLS,
    };

    Daemon();
public:
    static Daemon &GetInstance();
    std::string GetDBPath() const;
    std::string GetLogPath() const;
    std::string GetGRPCServerInterface() const;
    std::string GetGRPCServerPort() const;

    std::string GetIEXUrl() const;
    std::string GetIEXTocken() const;
    int GetIEXUpdateTimeout() const;
    std::vector<std::string> GetIEXUpdateSymbols() const;

private:
    void Init();

private:
    std::shared_ptr<Config> cfg;
    const additions::Section sectionDaemon;
    const additions::Section sectionGRPC;
    const additions::Section sectionIEX;
    additions::KeyManager<eKeys> keys;
};

} //namespace config
