cmake_minimum_required(VERSION 3.20)
project(OJ_Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(MySQL_INCLUDE_DIR "/usr/include/mysql/")
set(MySQL_LIBRARIES "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")

set(ctemplate_INCLUDE_DIR "/usr/local/include/ctemplate")
set(ctemplate_LIBRARIES "/usr/local/lib/libctemplate.so")

include_directories("../util")

add_executable(Server server.cpp)

target_link_libraries(Server ${MySQL_LIBRARIES} ${ctemplate_LIBRARIES} pthread)

