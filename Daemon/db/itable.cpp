#include "itable.h"
#include <chrono>

using namespace db;
using namespace table;
using namespace std;

std::recursive_mutex ITable::mtx;

ITable::ITable(const string &tableName, shared_ptr<SQLite::Database> dbConnector)
    : name(tableName)
    , db(dbConnector)
{
}

ITable::~ITable()
{
    // Waiting for finalizing of last transaction
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
}

bool ITable::IsExist()
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    return db->tableExists(GetName());
}

void ITable::Drop()
{
    RunTransaction("DROP TABLE IF EXISTS " + GetName());
}

string ITable::GetName() const
{
    return name;
}

std::shared_ptr<SQLite::Database> ITable::GetDB() const
{
    return db;
}

void ITable::RemoveAll()
{
    string cmd = "DELETE FROM " + GetName();
}

void ITable::GetColumnValue(const SQLite::Column &column, string *value) const
{
    *value = column.getString();
}

void ITable::GetColumnValue(const SQLite::Column &column, int *value) const
{
    *value = column.getInt();
}

void ITable::GetColumnValue(const SQLite::Column &column, int64_t *value) const
{
    *value = column.getInt64();
}

void ITable::GetColumnValue(const SQLite::Column &column, uint *value) const
{
    *value = column.getUInt();
}

void ITable::GetColumnValue(const SQLite::Column &column, bool *value) const
{
    *value = (column.getInt() != 0);
}

bool ITable::IsCellPresent(const string &filedName, const string &compareValue) const
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    string cmd = "SELECT EXISTS(SELECT 1 FROM " + GetName() + " " +
                 "WHERE " + filedName + " = \"" + compareValue + "\" " +
                 "LIMIT 1)";
    return  GetDB()->execAndGet(cmd).getInt() != 0;
}

void ITable::RunTransaction(const string &cmd)
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    SQLite::Transaction transaction(*GetDB());
    GetDB()->exec(cmd);
    transaction.commit();
}

void ITable::RunTransaction(std::unique_ptr<SQLite::Statement> statement)
{
    std::lock_guard<std::recursive_mutex> locker(GetMutex());
    SQLite::Transaction transaction(*GetDB());
    statement->exec();
    transaction.commit();
}

int64_t ITable::GetTimestamp() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
