#include "table_prices.h"
#include <plog/Log.h>
#include <boost/lexical_cast.hpp>
#include "table_companies.h"

using namespace db::table;
using namespace std;

Prices::Prices(std::shared_ptr<SQLite::Database> dbConnector)
    : ITable("prices", dbConnector)
    , fields({
{eFields::ID, "id"},
{eFields::COMPANY_ROWID, "company_rowid"},
{eFields::PRICE, "price"},
{eFields::TIMESTAMP, "timestamp"},
             })
{
}

void Prices::Create()
{
    string cmd = "PRAGMA foreign_keys=on; CREATE TABLE IF NOT EXISTS " + GetName() +
            "(" + fields[eFields::ID] + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
            fields[eFields::PRICE] + " REAL NOT NULL, " +
            fields[eFields::TIMESTAMP] + " INT NOT NULL, " +
            fields[eFields::COMPANY_ROWID] + " INTEGER NOT NULL, " +
            "FOREIGN KEY(" + fields[eFields::COMPANY_ROWID] + ") REFERENCES " + Companies::GetTableName() + "(" + Companies::GetFields().at(Companies::eFields::ID) + ")" +
            ")";
    RunTransaction(cmd);
}

void Prices::Init()
{
}

void Prices::Add(const int64_t &companyRowId, float price, int64_t timestamp)
{
    string cmd = "INSERT INTO " + GetName() + " (" +
            fields[eFields::COMPANY_ROWID] + ", " +
            fields[eFields::PRICE] + ", " +
            fields[eFields::TIMESTAMP] +
            ") VALUES (:rowid, :price, :time)";
    unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
    query->bind(":rowid", companyRowId);
    query->bind(":price", price);
    query->bind(":time", timestamp);

    RunTransaction(move(query));
}

void Prices::RemoveAllByCompanyRowID(int64_t companyRowId)
{
    RemoveRows(fields[eFields::COMPANY_ROWID], companyRowId);
}
