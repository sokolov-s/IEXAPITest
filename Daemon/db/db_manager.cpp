#include "db_manager.h"
#include "SQLiteCpp/Statement.h"
#include "config/config_daemon.h"
#include "plog/Log.h"

using namespace db;
using namespace std;

static const string dbPath = "./peakwork.db";

DBManager::DBManager()
    : db(make_shared<SQLite::Database>(config::Daemon::GetInstance().GetDBPath(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE))
    , tVersion(new table::DBVersion(db))
    , tCompanies(new table::Companies(db))
    , tPrices(new table::Prices(db))
{
    CreateAndInitAllTables();
    UpgradeVersion();
}

DBManager &DBManager::GetInstance()
{
    static DBManager db;
    return db;
}

table::Companies &DBManager::GetCompanies()
{
    return *tCompanies;
}

table::Prices &DBManager::GetPrices()
{
    return *tPrices;
}

void DBManager::CloseDB()
{
    if(db.get() != nullptr) {
        db.reset();
    }
}

void DBManager::UpgradeVersion()
{
    if(!tVersion->IsExist()) {
        PLOG_WARNING << "Can't check database version: Can't find table \"" << tVersion->GetName() << "\"";
        DropAndCreateAllTables();
    } else {
        try {
            size_t value = tVersion->GetDBVersion();
            if (GetDBVersion() < value) {
                PLOG_WARNING << "DB version is unknown";
                DropAndCreateAllTables();
            } else if(GetDBVersion() > value) {
                PLOG_INFO << "Upgrade database";
                Upgrade(value);
            }
        } catch(std::exception &e) {
            PLOG_ERROR << "Can't get database version : " << e.what();
            DropAndCreateAllTables();
        }
    }
}

void DBManager::CreateAndInitAllTables()
{
    PLOG_INFO << "Creating tables";
    tVersion->Create(GetDBVersion());
    tVersion->Init();

    tCompanies->Create();
    tCompanies->Init();

    tPrices->Create();
    tPrices->Init();
}

void DBManager::DropAllTables()
{
    PLOG_INFO << "Drop all tables";
    tVersion->Drop();
    tCompanies->Drop();
    tPrices->Drop();
}

void DBManager::DropAndCreateAllTables()
{
    PLOG_INFO << "Recreate all tables";
    DropAllTables();
    CreateAndInitAllTables();
}

void DBManager::Upgrade(size_t /*prevVersion*/)
{
    tVersion->UpdateDBVersion(GetDBVersion());
}

size_t DBManager::GetDBVersion() const
{
    return dbVersion;
}
