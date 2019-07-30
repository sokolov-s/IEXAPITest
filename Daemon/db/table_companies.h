#pragma once

#include "itable.h"
#include <string>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>
#include <map>

namespace db {
namespace table {

class Companies : public ITable
{
public:
    enum class eFields {
        ID = 0,
        UUID,
        NAME,
        LOGO,
    };
    using Fields = std::map<eFields, std::string>;

    Companies(std::shared_ptr<SQLite::Database> dbConnector);

    // ITable interface
    virtual void Create() override;
    virtual void Init() override;

    void Add(const std::string &uuid, const std::string &name, const std::string &logo);
    int64_t GetRowID(const std::string &uuid);

    void RemoveByRowID(const int64_t &rowId);

    std::string GetUUIDByRowID(const int64_t &rowId);
    std::string GetNameByRowID(const int64_t &rowId);

    bool IsUUIDPresent(const std::string &uuid) const;
    bool IsIDPresent(const int64_t &rowId) const;

//    std::vector<std::pair<std::string, std::string>> GetComponents() const;

    std::string GetLogoByRowID(const int64_t &rowId);
    void SetLogoByID(const int64_t &rowId, const std::string &link);

    static Fields GetFields() {return fields;}
    static std::string GetTableName() {return tableName;}
private:
    static const std::string tableName;
    static const Fields fields;
};

} //namespace table
} //namespace db
