#include "peakwork_server.h"
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include "config/config_daemon.h"

using namespace grpc;
using namespace grpcserver;
using namespace std;

PeakworkServiceImpl::PeakworkServiceImpl(IPeakworkService *handler)
    : handler(handler)
{
    assert(handler != nullptr);
}

PeakworkServiceImpl::~PeakworkServiceImpl()
{
    Stop();
}

void PeakworkServiceImpl::Start()
{
    std::string server_address(config::Daemon::GetInstance().GetGRPCServerInterface() + ":" + config::Daemon::GetInstance().GetGRPCServerPort());
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    server = builder.BuildAndStart();
}

void PeakworkServiceImpl::Stop()
{
    if(server) {
        server->Shutdown();
        server->Wait();
        server.release();
    }
}

Status PeakworkServiceImpl::GetCompanyInfo(ServerContext */*context*/,
                                           const GRPCServer::GetCompanyRequest *request,
                                           GRPCServer::GetCompanyResponse *response)
{
    try {
        vector<string> companies;
        int size = request->company().size();
        companies.reserve(static_cast<size_t>(size));
        for(int i = 0; i < size; ++i) {
            companies.push_back(request->company(i));
        }
        auto companiesInfo = handler->GetCompanyInfo(std::move(companies),
                                                     request->time_limit().IsInitialized() ? request->time_limit().timeframe() : "",
                                                     request->time_limit().IsInitialized() ? request->time_limit().date() : "");
        boost::property_tree::ptree jsonResult;
        for(const auto &info : companiesInfo) {
            boost::property_tree::ptree company;
            company.put("name", info.fullName);
            company.put("logo", info.logo);
            boost::property_tree::ptree prices;
            for(const auto &price: info.priceList) {
                boost::property_tree::ptree obj;
                obj.put("", price.price);
                prices.push_back(make_pair("", obj));
            }
            company.add_child("prices", prices);

            jsonResult.push_back(std::make_pair("", company));
        }
        ostringstream oss;
        boost::property_tree::write_json(oss, jsonResult, false);
        GRPCServer::Json *jsonData = new GRPCServer::Json();
        jsonData->set_data(oss.str());
        response->set_allocated_json_data(jsonData);

        return grpc::Status::OK;
    } catch(const std::exception &e) {
        PLOG_ERROR << e.what();
        return grpc::Status::CANCELLED;
    }
}
