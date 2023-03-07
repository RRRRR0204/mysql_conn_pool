#pragma once

#include "Connection.h"

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include <condition_variable>
#include <chrono>

// 连接池对象
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();

    // 给外部提供接口，获取一个可用的连接
    std::shared_ptr<Connection> getConnection();

private:
    // 单例模式
    ConnectionPool();

    // 加载数据库配置文件
    bool loadConfigFile();

    // 生产者线程函数
    void produceConnectionTask();

    // 用于定时清除空闲连接的扫描线程函数
    void scanConnectionTask();

    std::string _ip;       //< mysql的ip地址
    unsigned short _port;  //< mysql的端口号
    std::string _name;     //< mysql的登录名
    std::string _password; //< mysql登录密码
    std::string _dbname;   //< 数据库名称

    int _initSize;          //< 连接池初始连接数量
    int _maxSize;           //< 连接池最大连接数量
    int _maxIdleTime;       //< 连接池最大空闲时间
    int _connectionTimeout; //< 连接池获取连接的超时时间

    std::queue<Connection *> _connectionQue; //< 存储mysql连接的队列
    std::mutex _queueMutex;                  //< 维护队列的线程安全锁
    std::atomic_int _connectionCnt;          //< 记录连接池创建Connection的总数量
    std::condition_variable _cv;             //< 用于生产者和消费者通信的条件变量
};