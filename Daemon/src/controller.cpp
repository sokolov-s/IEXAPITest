#include "controller.h"
#include "db/db_manager.h"
#include "config/config_daemon.h"

#include <plog/Log.h>
#include <boost/bind.hpp>

using namespace daemonspace;
using namespace std;

Controller::Controller()
    : cfg(config::Daemon::GetInstance())
    , dbManager(db::DBManager::GetInstance())
    , rpcService(rpcservice::RPCService::GetInstance())
{
    plog::init(static_cast<plog::Severity>(LOG_LEVEL), cfg.GetLogPath().c_str(), 10 * 1024 * 1024, 10);
}

Controller::~Controller()
{
    Stop();
    PLOG_DEBUG << "Service stoped";
}

void Controller::Start()
{
    stopFlag.store(false);
    rpcService.Start(this);
    updatePricesThr = thread(bind(&Controller::UpdatePrices, this));
    PLOG_DEBUG << "Service started";
}

void Controller::Stop()
{
    stopFlag.store(true);
    {
         std::unique_lock<std::mutex> locker(mtx);
         stopCond.notify_one();
    }
    updatePricesThr.join();
    rpcService.Stop();
}

std::deque<common_data::CompanyInfo> Controller::GetCompanyInfo(std::vector<string> companies,
                                                                 const std::string &timeFrame,
                                                                 const std::string &date) const
{
    std::deque<common_data::CompanyInfo> result;
    for(const auto & company : companies) {
        result.push_back(rpcService.GetHTTPCompanyInfo(company, timeFrame, date));
    }
    return result;
}

void Controller::UpdatePrices()
{
    std::chrono::seconds updateTimeout(cfg.GetIEXUpdateTimeout());
    auto symbols(cfg.GetIEXUpdateSymbols());
    if(symbols.empty()) {
        return;
    }
    while(!stopFlag) {
        for(const auto &symbol : symbols) {
            auto info = rpcService.GetHTTPCompanyInfo(symbol);
            auto &compantTable = dbManager.GetCompanies();
            compantTable.Add(info.shortName, info.fullName, info.logo);
            auto rowId = compantTable.GetRowID(info.shortName);
            for(const auto &price : info.priceList) {
                dbManager.GetPrices().Add(rowId, price.price, price.dateTimestamp);
            }
        }
        std::unique_lock<std::mutex> locker(mtx);
        stopCond.wait_for(locker, std::chrono::seconds(), [this](){return stopFlag == true;});
    }
}

