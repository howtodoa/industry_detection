#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>

#ifdef SQLITEDB_EXPORTS
#define SQLITEDB_API __declspec(dllexport)
#else
#define SQLITEDB_API __declspec(dllimport)
#endif

namespace SqliteDB {
    class SQLITEDB_API DBOperation {
    public:
        // 初始化接口
        static int Initialize(const char * dbFilePath);

        static int Release();

        // 创建和删除表
        static int CreateTable(const char* tableName, const char* columnsDef);
        static int DropTable(const char* tableName);
        static int GetTableColumns(const char* tableName, std::vector<std::string>& outColumns);

        // 写入接口
        static int InsertRecord(const char* tableName, const std::vector<std::variant<int, double, std::string>>& values);
        static int UpdateRecordValue(const char* tableName, const char* strKey, const std::variant<int, double, std::string>& value, const char* condition);
        static int UpdateFullRecord(const char* tableName, const std::map<std::string, std::variant<int, double, std::string>>& values, const char* condition);

        // 读取接口
        static int GetRecordValue(const char* tableName, const char* strKey, const char* condition, std::variant<int, double, std::string>& outValue);
        static int GetFullRecord(const char* tableName, const char* condition, std::map<std::string, std::variant<int, double, std::string>>& outValues);
        static int GetMultipleRecords(const char* tableName, const char* condition, std::vector<std::map<std::string, std::variant<int, double, std::string>>>& outRecords);
        static int ExecuteQuery(const char* sqlQuery, std::vector<std::map<std::string, std::variant<int, double, std::string>>>& outRecords);
    };
}