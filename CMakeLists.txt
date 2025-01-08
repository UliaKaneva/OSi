cmake_minimum_required(VERSION 3.22)
project(Osi C)

set(CMAKE_C_STANDARD 99)

add_library(blocks2n_allocator SHARED laba4/blocks2n_allocator.c laba4/blocks2n_allocator.h)
target_include_directories(blocks2n_allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/laba4)

add_library(twin_allocator SHARED laba4/twin_allocator.c laba4/twin_allocator.h)
target_include_directories(twin_allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/laba4)

add_executable(Laba4 laba4/main.c)

# Линкуем исполняемый файл с библиотеками
target_link_libraries(Laba4 blocks2n_allocator twin_allocator)

# Указываем, где искать заголовочные файлы
target_include_directories(Laba4 PUBLIC ${CMAKE_SOURCE_DIR}/laba4)