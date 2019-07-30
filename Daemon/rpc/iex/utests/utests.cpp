#include <memory>
#include <mutex>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "config/config_daemon.h"
#include "rpc/iex/iex_client.h"

using namespace std;
static auto &cfg = config::Daemon::GetInstance();

TEST(IEX_UNIT_TEST, CheckHTTPResponse)
{
    iex::IEX_Client client;
    common_data::CompanyInfo info;
    ASSERT_NO_THROW(info = client.GetCompanyInfo("aapl"));
    EXPECT_STRCASEEQ(info.shortName.c_str(), "aapl");
    ASSERT_FALSE(info.fullName.empty());
    ASSERT_FALSE(info.priceList.empty());
    ASSERT_FALSE(info.logo.empty());
    ASSERT_GT(info.priceList.at(0).price, 0.000);
}
