#include <memory>
#include <mutex>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>

#include "rpc/rpc.h"
#include "peakwork_server.grpc.pb.h"
#include "config/config_daemon.h"

using namespace std;
static auto &cfg = config::Daemon::GetInstance();

TEST(GRPC_TEST, CheckGRPCServerResponse)
{
    string server = cfg.GetGRPCServerInterface() + ":" + cfg.GetGRPCServerPort();
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(server, grpc::InsecureChannelCredentials());
    ASSERT_TRUE(channel);
    std::unique_ptr<GRPCServer::Peakwork::Stub> stub = GRPCServer::Peakwork::NewStub(channel);
    ASSERT_TRUE(stub);

    GRPCServer::GetCompanyRequest request;
    request.add_company("AAPL");
    request.add_company("FB");
    GRPCServer::GetCompanyResponse response;
    grpc::ClientContext context;
    auto info = stub->GetCompanyInfo(&context, request, &response);
    ASSERT_TRUE(info.ok());
    ASSERT_TRUE(response.IsInitialized());
    ASSERT_TRUE(response.json_data().IsInitialized());
    ASSERT_FALSE(response.json_data().data().empty());
    cout << response.json_data().data() << endl;
}
