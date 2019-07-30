#pragma once

#include "grpc/peakwork_server.h"
#include "iex/iex_client.h"
#include <mutex>

namespace rpcservice {

class RPCService
{
    RPCService();
public:
    ~RPCService();
    static RPCService &GetInstance();

    void Start(grpcserver::IPeakworkService *peakworkServiceHandler);
    void Stop();

    common_data::CompanyInfo GetHTTPCompanyInfo(const std::string &shortName,
                                                const std::string &timeFrame = "",
                                                const std::string &date = "") const;


private:
    //TODO: change to shared_ptr
    grpcserver::IPeakworkService *userHandler;
    std::unique_ptr<grpcserver::PeakworkServiceImpl> grpcServer;
    std::unique_ptr<iex::IEX_Client> iexClient;
    std::mutex mtx;
    bool isStarted = false;
};

} //rpcservice
