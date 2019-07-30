#pragma once

#include "rpc/rpc.h"

#include <memory>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace common_data {
struct CompanyInfo;
}

namespace db {
class DBManager;
}

namespace config {
class Daemon;
}

namespace daemonspace {

class Controller : public grpcserver::IPeakworkService
{
public:
    Controller();
    ~Controller() override;

    void Start();
    void Stop();

    // IRPC
    virtual std::deque<common_data::CompanyInfo> GetCompanyInfo(std::vector<std::string> companies,
                                                                 const std::string &timeFrame,
                                                                 const std::string &date) const override;
private:
    void UpdatePrices();
private:
    config::Daemon &cfg;
    db::DBManager &dbManager;
    rpcservice::RPCService &rpcService;
    std::mutex mtx;
    std::thread updatePricesThr;
    std::atomic_bool stopFlag;
    std::condition_variable stopCond;
};

} //namespace daemon
