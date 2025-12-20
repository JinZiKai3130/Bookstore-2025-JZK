cmake_minimum_required(VERSION 3.10)
project(Bookstore VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# 添加可执行文件，命名为 code
add_executable(code ${SRC_FILES})

# 设置目标属性
target_include_directories(code PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 将可执行文件输出到项目根目录
set_target_properties(code PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# 添加编译器警告选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE 
        -Wall
        -Wextra
        -Wpedantic
        -Wreturn-local-addr
        # -Werror  # 如果需要将所有警告视为错误，取消注释这行
    )
endif()

# 针对 MSVC 编译器
if(MSVC)
    target_compile_options(code PRIVATE /W4)
endif()

# 安装规则（可选）
install(TARGETS code
    RUNTIME DESTINATION bin
)