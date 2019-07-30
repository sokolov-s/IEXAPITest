#include "table_db_version.h"
#include <boost/lexical_cast.hpp>

using namespace db;
using namespace table;
using namespace std;

DBVersion::DBVersion(std::shared_ptr<SQLite::Database> dbConnector)
    : ITable("db_version", dbConnector)
    , fields({
{eFields::ID, "id"},
{eFields::VERSION, "version"},
             })
{
}

void DBVersion::Create(size_t version)
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    Create();
    if(!IsVersionPresent()) {
        string cmd = "INSERT INTO " + GetName() +
                     " (" + fields[eFields::VERSION] + ")" +
                     "VALUES (" + boost::lexical_cast<string>(version) + ")";
        RunTransaction(cmd);
    }
}

void DBVersion::Create()
{
    string cmd = "CREATE TABLE IF NOT EXISTS " + GetName() + " (" +
                 fields[eFields::ID] + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                 fields[eFields::VERSION] + " INT NOT NULL" +
                 ")";
    RunTransaction(cmd);
}

size_t DBVersion::GetDBVersion()
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "SELECT " + fields[eFields::VERSION] + " FROM " + GetName();
    return GetDB()->execAndGet(cmd).getUInt();
}

void DBVersion::UpdateDBVersion(size_t newVersion)
{
    string cmd = "UPDATE " + GetName() + "SET " + fields[eFields::VERSION] + " = " + boost::lexical_cast<string>(newVersion);
    RunTransaction(cmd);
}

bool DBVersion::IsVersionPresent()
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "SELECT EXISTS(SELECT 1 FROM " + GetName() + " LIMIT 1)";
    return  GetDB()->execAndGet(cmd).getInt() != 0;
}
