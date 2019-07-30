#pragma once

#include "itable.h"
#include <string>
#include <vector>
#include <set>

namespace db {
namespace table {

class Prices : public ITable
{
    enum class eFields {
        ID = 0,
        COMPANY_ROWID,
        PRICE,
        TIMESTAMP,
    };
public:
    Prices(std::shared_ptr<SQLite::Database> dbConnector);

    // ITable interface
    virtual void Create() override;
    virtual void Init() override;

    void Add(const int64_t &companyRowId, float price, int64_t timestamp);
    void RemoveAllByCompanyRowID(int64_t companyRowId);
//    std::vector<int64_t> Get(const std::string &uuid);
private:
    std::map<eFields, std::string> fields;
};

} //namespace table
} //namespace db
