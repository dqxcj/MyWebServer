cmake_minimum_required(VERSION 3.0)

project(WEBSERVERLIB)

# 获取cpp源文件列表
aux_source_directory(./ SRC)
# 设置头文件路径
include_directories(${INCLUDE_PATH})
# 设置生成静态库的保存路径
set(LIBRARY_OUTPUT_PATH ${LIB_PATH})
# 生成静态库
add_library(${WEBSERVER_LIB} STATIC ${SRC})
message("lib_name: ${WEBSERVER_LIB}")
message("src: ${SRC}")