cmake_minimum_required(VERSION 3.0)

project(mysql_conn_pool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

include_directories(/usr/lib64/mysql)
link_directories(/usr/lib64/mysql)

aux_source_directory(. SRC_LIST)

# 指定由哪些文件生成可执行文件
add_executable(main ${SRC_LIST})

target_link_libraries(main mysqlclient)

