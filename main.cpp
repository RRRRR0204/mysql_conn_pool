#include "Connection.h"
#include "ConnectionPool.h"

#include <iostream>
using namespace std;

int main()
{
    clock_t begin = clock();

    thread t1([]() {
        ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; ++i)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name, age, sex) values('%s', %d, '%s')",
                    "zhang san", 20, "male");

            // Connection conn;
            // conn.connect("127.0.0.1", 3306, "root", "Ab123456", "test");
            // conn.update(sql);

            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });

    thread t2([]() {
        ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; ++i)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name, age, sex) values('%s', %d, '%s')",
                    "zhang san", 20, "male");
                    
            // Connection conn;
            // conn.connect("127.0.0.1", 3306, "root", "Ab123456", "test");
            // conn.update(sql);

            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });

    thread t3([]() {
        ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; ++i)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name, age, sex) values('%s', %d, '%s')",
                    "zhang san", 20, "male");
                    
            // Connection conn;
            // conn.connect("127.0.0.1", 3306, "root", "Ab123456", "test");
            // conn.update(sql);

            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });

    thread t4([]() {
        ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; ++i)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name, age, sex) values('%s', %d, '%s')",
                    "zhang san", 20, "male");
                    
            // Connection conn;
            // conn.connect("127.0.0.1", 3306, "root", "Ab123456", "test");
            // conn.update(sql);

            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    clock_t end = clock();
    std::cout << (end - begin) << "ms" << std::endl;

#if 0
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 10000; ++i)
    {
        char sql[1024] = {0};
        sprintf(sql, "insert into user(name, age, sex) values('%s', %d, '%s')",
                "zhang san", 20, "male");

        // Connection conn;
        // conn.connect("127.0.0.1", 3306, "root", "Ab123456", "test");
        // conn.update(sql);

        std::shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    }
#endif

    return 0;
}