cmake_minimum_required(VERSION 3.10)
project(Bookstore VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# 添加可执行文件
add_executable(bookstore ${SRC_FILES})

# 可选的：如果项目需要编译为库而不是可执行文件
# add_library(bookstore_lib STATIC ${SRC_FILES})

# 设置目标属性
target_include_directories(bookstore PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 如果项目需要链接其他库，可以在这里添加
# find_package(SomePackage REQUIRED)
# target_link_libraries(bookstore SomePackage::SomeComponent)

# 安装规则（可选）
install(TARGETS bookstore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)