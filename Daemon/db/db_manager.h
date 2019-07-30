#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <map>
#include <memory>
#include "itable.h"
#include "table_companies.h"
#include "table_prices.h"
#include "table_db_version.h"

namespace db {

class DBManager
{
    DBManager();
public:
    static DBManager & GetInstance();

    table::Companies & GetCompanies();
    table::Prices & GetPrices();

    void CloseDB();
private:
    void UpgradeVersion();
    void CreateAndInitAllTables();
    void DropAllTables();
    void DropAndCreateAllTables();
    void Upgrade(size_t prevVersion);
    size_t GetDBVersion() const;

private:
    std::shared_ptr<SQLite::Database> db;
    std::unique_ptr<table::DBVersion> tVersion;
    std::unique_ptr<table::Companies> tCompanies;
    std::unique_ptr<table::Prices> tPrices;
    size_t dbVersion = 1;
};

} //namespace db
