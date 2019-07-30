#include "iex_client.h"
#include "config/config_daemon.h"
#include "common_utils/http_transaction.h"
#include "common_utils/http_types.h"

#include <plog/Log.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace iex;
using namespace common_utils;
using namespace std;

IEX_Client::IEX_Client()
    : cfg(config::Daemon::GetInstance())
{
}

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

common_data::CompanyInfo IEX_Client::GetCompanyInfo(const std::string &shortName) const
{
    try {
        auto url = CreateUrl(shortName, "quote");
        auto data = HTTPTransaction::PerformGet(url);
        PLOG_DEBUG << "Http reques : " << url << "\t returned data : " << data->str();
        auto info = ParseCompanyInfo(move(*data));
        info.logo = GetCompanyLogoLink(shortName);
        return info;
    } catch(const HTTPException &ex) {
        PLOG_ERROR << ex.what();
        return {};
    }
}

common_data::CompanyInfo IEX_Client::GetCompanyInfo(const string &shortName, const string &timeFrame) const
{
    try {
        auto url = CreateUrl(shortName, "chart/" + timeFrame);;
        auto data = HTTPTransaction::PerformGet(url);
        PLOG_DEBUG << "Http reques : " << url << "\t returned data : " << data->str();
        auto info = ParseCompanyInfo(move(*data));
        info.logo = GetCompanyLogoLink(shortName);
        return info;
    } catch(const HTTPException &ex) {
        PLOG_ERROR << ex.what();
        return {};
    }
}

common_data::CompanyInfo IEX_Client::GetCompanyInfo(const string &shortName,
                                                    const string &timeFrame,
                                                    const string &date) const
{
    try {
        auto url = CreateUrl(shortName, "chart/" + timeFrame + "/" + date);
        auto data = HTTPTransaction::PerformGet(url);
        PLOG_DEBUG << "Http reques : " << url << "\t returned data : " << data->str();
        auto info = ParseCompanyInfo(move(*data));
        info.logo = GetCompanyLogoLink(shortName);
        return info;
    } catch(const HTTPException &ex) {
        PLOG_ERROR << ex.what();
        return {};
    }
}

string IEX_Client::CreateUrl(const std::string &companySymbol, const std::string &type) const
{
    return GetBaseUrl() + "/stable/stock/" + companySymbol + "/" + type + "?token=" + GetTocken();
}

string IEX_Client::GetBaseUrl() const
{
    return cfg.GetIEXUrl();
}

string IEX_Client::GetTocken() const
{
    return cfg.GetIEXTocken();
}

string IEX_Client::GetCompanyLogoLink(const std::string &shortName) const
{
    try {
        auto url = CreateUrl(shortName, "logo");
        auto data = HTTPTransaction::PerformGet(url);
        PLOG_DEBUG << "Http reques : " << url << "\t returned data : " << data->str();
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(*data, pt);
        return pt.get<string>("url");
    } catch(const HTTPException &ex) {
        PLOG_ERROR << ex.what();
        return "";
    }
}

common_data::CompanyInfo IEX_Client::ParseCompanyInfo(stringstream info) const
{
    PLOG_DEBUG << "Parse company information : " << info.str();
    common_data::CompanyInfo result;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(info, pt);
    result.fullName = pt.get<string>("companyName");
    result.shortName = pt.get<string>("symbol");
    result.priceList.push_back({pt.get<float>("latestPrice"), pt.get<int64_t>("latestUpdate"), pt.get<string>("latestUpdate")});
    return result;
}
