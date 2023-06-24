# 设置编译器
# 设置 C 编译器路径
set(CMAKE_C_COMPILER "/usr/bin/clang-14")

# 设置 C++ 编译器路径
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-14")

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")  # 设置 C 编译器的优化级别为 O2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")  # 设置 C++ 编译器的优化级别为 O2

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")  # 启用 C 编译器的调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")  # 启用 C++ 编译器的调试信息
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

# 添加子目录
add_subdirectory(src)
add_subdirectory(test)

# 设置build_support目录
# Expected directory structure.
set(PINE_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(PINE_CLANG_SEARCH_PATH "/usr/bin" "/usr/local/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")


# runs clang format and updates files in place.
# clang-format
# attempt to find the binary if user did not specify
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-14
        HINTS /usr/bin)

# set(CLANG_FORMAT_BIN "/usr/bin/clang-format")
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Pine/main couldn't find clang-format.")
else ()
    message(STATUS "Pine/main found clang-format at ${CLANG_FORMAT_BIN}")
endif ()

##########################################
# "make format"
# "make check-format"
##########################################
string(CONCAT PINE_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
        )

# runs clang format and updates files in place.
add_custom_target(format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${PINE_FORMAT_DIRS}
        --fix
        --quiet
        )

# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${PINE_FORMAT_DIRS}
        --quiet
        )

# clang-tidy
# attempt to find the binary if user did not specify
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-fidy-14
        HINTS ${PINE_CLANG_SEARCH_PATH})
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "Pine/main couldn't find clang-tidy.")
else ()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "Pine/main found clang-fidy at ${CLANG_TIDY_BIN}")
endif ()

###########################################################
# "make clang-tidy" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(clang-tidy
        ${PINE_BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )


# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${PINE_BUILD_SUPPORT_DIR})
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "Pine/main couldn't find cpplint.")
else ()
    message(STATUS "Pine/main found cpplint at ${CPPLINT_BIN}")
endif ()


##########################################
# "make cpplint"
##########################################

file(GLOB_RECURSE PINE_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
        )

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(cpplint echo '${PINE_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting
        )
