#pragma once

#include <mysql/mysql.h>
#include <string>

// MySQL数据库的操作
class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    bool connect(std::string ip, unsigned short port, std::string user,
                 std::string password, std::string dbname);

    // 更新操作
    bool update(std::string sql);
    // 查询操作
    MYSQL_RES *query(std::string sql);

private:
    MYSQL *_conn;
};