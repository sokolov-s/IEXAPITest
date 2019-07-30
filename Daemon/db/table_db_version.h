#pragma once

#include "itable.h"
#include <string>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>
#include <map>

namespace db {
namespace table {

class DBVersion : public ITable
{
    enum class eFields {
        ID = 0,
        VERSION,
    };
public:
    DBVersion(std::shared_ptr<SQLite::Database> dbConnector);

    void Create(size_t version);
    size_t GetDBVersion();
    void UpdateDBVersion(size_t newVersion);
private:
    // ITable interface
    virtual void Create() override;

    bool IsVersionPresent();
private:
    std::map <eFields, std::string> fields;
};

} //namespace table
} //namespace db
