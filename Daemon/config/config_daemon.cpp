#include "config_daemon.h"
#include "common_utils/common_functions.h"

using namespace config;
using namespace additions;
using namespace std;

namespace {
    const string logFile = "./log.txt";
    const string dbFile = "./peakwork.db";
    const string grpcInterface = "0.0.0.0";
    const string grpcPort = "50050";
    const int updateTime = 3;
    const string iexUrl = "https://sandbox.iexapis.com";
    const string iexToken = "Tpk_cf32bcdd9dac4ee586469a167121bda2";
    const std::vector<std::string> updateSymbols = {"aapl", "fb"};
}

Daemon::Daemon()
    : cfg(ConfigManager::GetInstance().GetConfig("daemon.cfg"))
    , sectionDaemon("daemon")
    , sectionGRPC("grpc_server")
    , sectionIEX("iex_api")
    , keys({
{sectionDaemon, eKeys::LOG_PATH, "log_path"},
{sectionDaemon, eKeys::DB_PATH,  "db_path"},
{sectionGRPC, eKeys::INTERFACE, "interface"},
{sectionGRPC, eKeys::PORT, "port"},
{sectionIEX, eKeys::IEX_URL,  "url"},
{sectionIEX, eKeys::IEX_TOCKEN,  "tocken"},
{sectionIEX, eKeys::IEX_UPDATE_PRICE_SECOND,  "update_price_timeout_sec"},
{sectionIEX, eKeys::IEX_UPDATE_SYMBOLS,  "update_symbols"},
           })
{
    Init();
}

Daemon &Daemon::GetInstance()
{
    static Daemon daemonCfg;
    return daemonCfg;
}

void Daemon::Init()
{
    cfg->WriteString(keys(sectionDaemon, eKeys::LOG_PATH), GetLogPath());
    cfg->WriteString(keys(sectionDaemon, eKeys::DB_PATH), GetDBPath());
    cfg->WriteString(keys(sectionGRPC, eKeys::INTERFACE), GetGRPCServerInterface());
    cfg->WriteString(keys(sectionGRPC, eKeys::PORT), GetGRPCServerPort());
    cfg->WriteString(keys(sectionIEX, eKeys::IEX_URL), GetIEXUrl());
    cfg->WriteString(keys(sectionIEX, eKeys::IEX_TOCKEN), GetIEXTocken());
    cfg->WriteInt(keys(sectionIEX, eKeys::IEX_UPDATE_PRICE_SECOND), GetIEXUpdateTimeout());
    cfg->Write(keys(sectionIEX, eKeys::IEX_UPDATE_SYMBOLS), GetIEXUpdateSymbols());
}

string Daemon::GetDBPath() const
{
    return cfg->GetString(keys(sectionDaemon, eKeys::DB_PATH), dbFile);
}

string Daemon::GetLogPath() const
{
    return cfg->GetString(keys(sectionDaemon, eKeys::LOG_PATH), logFile);
}

string Daemon::GetGRPCServerInterface() const
{
    return cfg->GetString(keys(sectionGRPC, eKeys::INTERFACE), grpcInterface);
}

string Daemon::GetGRPCServerPort() const
{
    return cfg->GetString(keys(sectionGRPC, eKeys::PORT), grpcPort);
}

string Daemon::GetIEXUrl() const
{
    return cfg->GetString(keys(sectionIEX, eKeys::IEX_URL), iexUrl);
}

string Daemon::GetIEXTocken() const
{
    return cfg->GetString(keys(sectionIEX, eKeys::IEX_TOCKEN), iexToken);
}

int Daemon::GetIEXUpdateTimeout() const
{
    return cfg->GetInt(keys(sectionIEX, eKeys::IEX_UPDATE_PRICE_SECOND), updateTime);
}

std::vector<string> Daemon::GetIEXUpdateSymbols() const
{
    return cfg->GetArray(keys(sectionIEX, eKeys::IEX_UPDATE_PRICE_SECOND), updateSymbols);
}
