cmake_minimum_required(VERSION 3.0)
project(TEST)

# 头文件路径
include_directories(${INCLUDE_PATH})
# 指定要链接的静态库
link_libraries(${WEBSERVER_LIB})
# 设置可执行程序的生成路径
set(EXECUTABLE_OUTPUT_PATH ${EXEC_PATH})
# 生成可执行程序
add_executable(${APP_NAME_SERV} server.cpp)
add_executable(${APP_NAME_CLNT} client.cpp)
add_executable(${APP_NAME_TEST} test.cpp)

