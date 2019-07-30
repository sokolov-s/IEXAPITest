#pragma once

#include "common_data/company_info.h"
#include <string>

namespace config {
class Daemon;
}

namespace iex
{

class IEX_Client
{
public:
    IEX_Client();
    common_data::CompanyInfo GetCompanyInfo(const std::string &shortName) const;
    common_data::CompanyInfo GetCompanyInfo(const std::string &shortName, const std::string &timeFrame) const;
    common_data::CompanyInfo GetCompanyInfo(const std::string &shortName,
                                            const std::string &timeFrame,
                                            const std::string &date) const;

private:
    std::string CreateUrl(const std::string &companySymbol, const std::string &type) const;
    std::string GetBaseUrl() const;
    std::string GetTocken() const;
    std::string GetCompanyLogoLink(const std::string &shortName) const;
    common_data::CompanyInfo ParseCompanyInfo(std::stringstream info) const;

private:
    const config::Daemon &cfg;
};

} //namespace iex
