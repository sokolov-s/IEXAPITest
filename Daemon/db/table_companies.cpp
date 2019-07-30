#include "table_companies.h"
#include <chrono>

using namespace db;
using namespace table;
using namespace std;

const string Companies::tableName = "companies";

const Companies::Fields Companies::fields = {
    {eFields::ID, "id"},
    {eFields::UUID, "uuid"},
    {eFields::NAME, "full_name"},
    {eFields::LOGO, "logo"},
};

Companies::Companies(std::shared_ptr<SQLite::Database> dbConnector)
    : ITable(tableName, dbConnector)
{
}

void Companies::Create()
{
    string cmd = "CREATE TABLE IF NOT EXISTS " + GetName() +
                  "(" + fields.at(eFields::ID) + " INTEGER PRIMARY KEY AUTOINCREMENT," +
                  fields.at(eFields::UUID) + " TEXT NOT NULL UNIQUE, " +
                  fields.at(eFields::NAME) + " TEXT NOT NULL, " +
                  fields.at(eFields::LOGO) + " TEXT " +
                  ")";
    RunTransaction(cmd);
}

void Companies::Init()
{
}

void Companies::Add(const string &uuid, const string &name, const string &logo)
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "INSERT OR IGNORE INTO " + GetName() + " " +
                 "(" + fields.at(eFields::UUID) +
                 ", " + fields.at(eFields::NAME) +
                 ", " + fields.at(eFields::LOGO) +
                 ") " +
                 " VALUES (:uuid, :name, :logo)";
    unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
    query->bind(":uuid", uuid);
    query->bind(":name", name);
    query->bind(":logo", logo);
    RunTransaction(std::move(query));
}

int64_t Companies::GetRowID(const string &uuid)
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "SELECT " + fields.at(eFields::ID) + " " +
                 "FROM " + GetName() + " " +
                 "WHERE " + fields.at(eFields::UUID) + " = \"" + uuid + "\" ";
    int64_t result = GetDB()->execAndGet(cmd).getInt64();
    return  result;
}

string Companies::GetUUIDByRowID(const int64_t &rowId)
{
    return GetFieldValue<string, long long>(fields.at(eFields::UUID), fields.at(eFields::ID), rowId);
}

string Companies::GetNameByRowID(const int64_t &rowId)
{
    return GetFieldValue<string, long long>(fields.at(eFields::NAME), fields.at(eFields::ID), rowId);
}

string Companies::GetLogoByRowID(const int64_t &rowId)
{
    return GetFieldValue<string, long long>(fields.at(eFields::LOGO), fields.at(eFields::ID), rowId);
}

void Companies::SetLogoByID(const int64_t &rowId, const string &link)
{
    UpdateField<string, long long>(fields.at(eFields::LOGO), link, fields.at(eFields::ID), rowId);
}

bool Companies::IsUUIDPresent(const string &uuid) const
{
    return  IsCellPresent(fields.at(eFields::UUID), uuid);
}

bool Companies::IsIDPresent(const int64_t &rowId) const
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "SELECT EXISTS(SELECT 1 FROM " + GetName() + " " +
                 "WHERE " + fields.at(eFields::ID) + " = " + std::to_string(rowId) + " " +
                 "LIMIT 1)";
    bool result = GetDB()->execAndGet(cmd).getInt() != 0;
    return  result;
}

void Companies::RemoveByRowID(const int64_t &rowId)
{
    RemoveRows<long long>(fields.at(eFields::ID), rowId);
}
