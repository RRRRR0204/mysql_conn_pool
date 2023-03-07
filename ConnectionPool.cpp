#include "ConnectionPool.h"
#include "public.h"
#include "json.hpp"
using json = nlohmann::json;

#include <fstream>

// 线程安全的懒汉单例模式
ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

ConnectionPool::ConnectionPool()
{
    // 加载配置项
    if (!loadConfigFile())
    {
        LOG("load config file error!");
        return;
    }

    // 创建初始连接
    for (int i = 0; i < _initSize; ++i)
    {
        Connection *conn = new Connection();
        conn->connect(_ip, _port, _name, _password, _dbname);
        conn->refreshAliveTime(); // 刷新一下起始空闲时间
        _connectionQue.push(conn);
        ++_connectionCnt;
    }

    // 启动生产者线程
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动扫描线程
    std::thread scan(std::bind(&ConnectionPool::scanConnectionTask, this));
    scan.detach();
}

// 加载数据库配置文件
bool ConnectionPool::loadConfigFile()
{
    std::ifstream file("mysqlConfig.json");
    json conf = json::parse(file);

    _ip = conf["ip"];
    _port = conf["port"];
    _name = conf["name"];
    _password = conf["password"];
    _dbname = conf["dbname"];

    _initSize = conf["initSize"];
    _maxSize = conf["maxSize"];
    _maxIdleTime = conf["maxIdleTime"];
    _connectionTimeout = conf["connectionTimeout"];

    return true;
}

// 生产者线程函数
void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (!_connectionQue.empty())
        {
            _cv.wait(lock); // 队列不空，此处生产线程进入等待状态
        }

        // 连接数还没达到上限，继续创建连接
        if (_connectionCnt < _maxSize)
        {
            Connection *conn = new Connection();
            conn->connect(_ip, _port, _name, _password, _dbname);
            conn->refreshAliveTime(); // 刷新一下起始空闲时间
            _connectionQue.push(conn);
            ++_connectionCnt;
        }

        // 通知消费者线程
        _cv.notify_all();
    }
}

// 给外部提供接口，获取一个可用的连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    while (_connectionQue.empty())
    {
        if (std::cv_status::timeout == _cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQue.empty())
            {
                LOG("ConnectionPool::getConnection timeout!");
                return nullptr;
            }
        }
    }

    /*
    给用户返回一个智能指针，自定义资源回收的方式：
    将Connection归还到queue中
    */
    std::shared_ptr<Connection> sp(_connectionQue.front(),
                                   [&](Connection *cp)
                                   {
                                       // 这里是在服务器应用线程中，要保证线程安全
                                       std::unique_lock<std::mutex> lock(_queueMutex);
                                       cp->refreshAliveTime(); // 刷新一下起始空闲时间
                                       _connectionQue.push(cp);
                                   });
    _connectionQue.pop();
    _cv.notify_all();

    return sp;
}

// 用于定时清除空闲连接的扫描线程函数
void ConnectionPool::scanConnectionTask()
{
    for (;;)
    {
        // sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

        // 扫描整个队列
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            Connection *conn = _connectionQue.front();
            if (conn->getAliveTime() >= (_maxIdleTime * 1000))
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete conn; // 调用~Connection释放连接
            }
            else
            {
                break; // 如果队头没有超时，后面肯定也没超时
            }
        }
    }
}