#include "rpc.h"
#include <functional>

using namespace rpcservice;
using namespace std;

RPCService::RPCService()
    : iexClient(new iex::IEX_Client())
{
}

RPCService::~RPCService()
{
    Stop();
}

RPCService &RPCService::GetInstance()
{
    static RPCService rpc;
    return rpc;
}

void RPCService::Start(grpcserver::IPeakworkService *peakworkServiceHandler)
{
    lock_guard<std::mutex> locker(mtx);
    if(isStarted) {
        return;
    }
    userHandler = peakworkServiceHandler;
    grpcServer.reset(new grpcserver::PeakworkServiceImpl(userHandler));
    assert(grpcServer != nullptr);
    grpcServer->Start();
    isStarted = true;
}

void RPCService::Stop()
{
    lock_guard<std::mutex> locker(mtx);
    grpcServer.release();
    userHandler = nullptr;
    isStarted = false;
}

common_data::CompanyInfo RPCService::GetHTTPCompanyInfo(const string &shortName,
                                                    const string &timeFrame,
                                                    const string &date) const
{
    if(timeFrame.empty() && date.empty()) {
        return iexClient->GetCompanyInfo(shortName);
    } else if (date.empty()) {
        return iexClient->GetCompanyInfo(shortName, timeFrame);
    } else {
        return iexClient->GetCompanyInfo(shortName, timeFrame, date);
    }
}
