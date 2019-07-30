#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <shared_mutex>
#include <mutex>
#include <sstream>

namespace db {
namespace table {

class ITable
{
public:
    ITable(const std::string &tableName, std::shared_ptr<SQLite::Database> dbConnector);
    virtual ~ITable();
    virtual bool IsExist();
    virtual void Create() = 0;
    virtual void Drop();
    virtual void Init() {}

    std::string GetName() const;
    std::shared_ptr<SQLite::Database> GetDB() const;

    void RemoveAll();

protected:
    void GetColumnValue(const SQLite::Column &column, std::string *value) const;
    void GetColumnValue(const SQLite::Column &column, int *value) const;
    void GetColumnValue(const SQLite::Column &column, int64_t *value) const;
    void GetColumnValue(const SQLite::Column &column, uint *value) const;
    void GetColumnValue(const SQLite::Column &column, bool *value) const;

    bool IsCellPresent(const std::string &filedName, const std::string &compareValue) const;

    template<typename ReturnType, typename CompareType>
    ReturnType GetFieldValue(const std::string &fieldName, const std::string &filterFieldName, const CompareType &value) const
    {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        auto selectQueryField(CreateGetFieldQuery<CompareType>(fieldName, filterFieldName, value));
        ReturnType result;
        if(selectQueryField->executeStep()) {
            if(selectQueryField->getColumn(0).isNull()) {
                throw std::out_of_range("Asked results is NULL");
            }
            GetColumnValue(selectQueryField->getColumn(0), &result);
        } else {
            throw std::logic_error("There is no one results");
        }
        if(selectQueryField->executeStep()) {
            std::ostringstream msg;
            msg << "There are more than one results for field=" << filterFieldName <<
                  " and value=" << value;
            throw std::logic_error(msg.str());
        }
        return result;
    }

    template<typename ReturnType>
    std::vector<ReturnType> GetFieldValueList(const std::string &fieldName) const
    {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        auto selectQueryField(CreateGetFieldQuery(fieldName));
        std::vector<ReturnType> result;
        while(selectQueryField->executeStep()) {
            ReturnType columnValue;
            GetColumnValue(selectQueryField->getColumn(0), &columnValue);
            result.push_back(std::move(columnValue));
        }
        return result;
    }

    template<typename ReturnType, typename CompareType>
    std::vector<ReturnType> GetFieldValueList(const std::string &fieldName, const std::string &filterFieldName, const CompareType &value) const
    {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        auto selectQueryField(CreateGetFieldQuery<CompareType>(fieldName, filterFieldName, value));
        std::vector<ReturnType> result;
        while(selectQueryField->executeStep()) {
            ReturnType columnValue;
            GetColumnValue(selectQueryField->getColumn(0), &columnValue);
            result.push_back(std::move(columnValue));
        }
        return result;
    }

    template<typename UpdateType, typename CompareType>
    void UpdateField(const std::string &fieldName, const UpdateType &newValue, const std::string &searchFieldName, const CompareType &searchValue) {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        std::string cmd = "UPDATE " + GetName() + " SET " +
                     fieldName + " = :newValue " +
                     "WHERE " + searchFieldName + " = :searchValue";
        std::unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
        query->bind(":newValue", newValue);
        query->bind(":searchValue", searchValue);
        RunTransaction(std::move(query));
    }

    template<typename CompareType>
    void RemoveRows(const std::string &searchFieldName, const CompareType &searchValue) {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        std::string cmd = "DELETE FROM " + GetName() + " " +
                     "WHERE " + searchFieldName + " = :searchValue";
        std::unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
        query->bind(":searchValue", searchValue);
        RunTransaction(std::move(query));
    }

    void RunTransaction(const std::string &cmd);
    void RunTransaction(std::unique_ptr<SQLite::Statement> statement);
    int64_t GetTimestamp() const;

    std::recursive_mutex & GetMutex() const {
        return mtx;
    }

private:
    template<typename CompareType>
    std::unique_ptr<SQLite::Statement> CreateGetFieldQuery(const std::string &fieldName,
                                                           const std::string &filterFieldName,
                                                           const CompareType &value) const
    {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        std::string cmd = "SELECT " + fieldName + " FROM " + GetName() + " " +
                     "WHERE " + filterFieldName + " = :compareValue";
        std::unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
        query->bind(":compareValue", value);
        return query;
    }

    std::unique_ptr<SQLite::Statement> CreateGetFieldQuery(const std::string &fieldName) const
    {
        std::lock_guard<std::recursive_mutex> locker(GetMutex());
        std::string cmd = "SELECT " + fieldName + " FROM " + GetName();
        std::unique_ptr<SQLite::Statement> query (new SQLite::Statement(*GetDB(), cmd));
        return query;
    }

private:
    std::string name;
    std::shared_ptr<SQLite::Database> db;
    static std::recursive_mutex mtx;
};

} //namespace table
} //namespace db

