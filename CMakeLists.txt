# 设置编译器
# 设置 C 编译器路径
set(CMAKE_C_COMPILER "/usr/bin/clang-14")

# 设置 C++ 编译器路径
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-14")

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")  # 设置 C 编译器的优化级别为 O2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")  # 设置 C++ 编译器的优化级别为 O2

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")  # 启用 C 编译器的调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")  # 启用 C++ 编译器的调试信息
# 链接linux线程库pthread
link_libraries(pthread)

cmake_minimum_required(VERSION 3.0)                                 # 最低cmake版本
project(MyWebServer)                                        # 项目名称

# 定义变量
# 静态库生成路径
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 测试程序生成路径
set(EXEC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# 头文件路径
set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
# 静态库的名字
set(WEBSERVER_LIB server)
# 可执行程序的名字
set(APP_NAME_SERV serv)
set(APP_NAME_CLNT clnt)
set(APP_NAME_TEST test)

# 添加子目录
add_subdirectory(src)
add_subdirectory(test)
