# 设置所需的 CMake 最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和使用的语言
project(template C)

# 设定 C 标准为 C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 添加可执行文件，并指定源文件
add_executable(${PROJECT_NAME}
    src/main.c
)

# 仅对当前目标生效的头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 基础警告选项（可按需调整）
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
