#pragma once

#include <string>
#include <deque>

namespace common_data {

struct Price {
    float price;
    int64_t dateTimestamp;
    std::string dateString;
};

struct CompanyInfo
{
    std::string shortName;
    std::string fullName;
    std::string logo;
    std::deque<Price> priceList;
};


} //namespace data
