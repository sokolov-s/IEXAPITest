#pragma once

#include <string>
#include <grpc++/grpc++.h>
#include <peakwork_server.grpc.pb.h>
#include <plog/Log.h>
#include "common_data/company_info.h"
#include <deque>

namespace grpcserver {

class IPeakworkService;

class PeakworkServiceImpl final : public GRPCServer::Peakwork::Service
{
public:
    PeakworkServiceImpl(IPeakworkService *handler);
    ~PeakworkServiceImpl() override;

    void Start();
    void Stop();

private:
    virtual ::grpc::Status GetCompanyInfo(::grpc::ServerContext* context,
                                          const ::GRPCServer::GetCompanyRequest* request,
                                          ::GRPCServer::GetCompanyResponse* response) override;

private:
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<grpc::Server> server;
    IPeakworkService *handler;
};


class IPeakworkService : public std::enable_shared_from_this<IPeakworkService> {
public:
    virtual ~IPeakworkService() = default;
    std::shared_ptr<IPeakworkService> GetSharedPtr() {return shared_from_this();}
    virtual std::deque<common_data::CompanyInfo> GetCompanyInfo(std::vector<std::string> companies,
                                                                const std::string &timeFrame,
                                                                const std::string &date) const = 0;
};


} //namespace grpcclient

