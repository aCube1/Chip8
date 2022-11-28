cmake_minimum_required(VERSION 3.16)
project(Chip8 C)

include(cmake/base.cmake)
include(cmake/warnings.cmake)
include(cmake/libraries.cmake)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    src/main.c
    src/core.c
    src/display.c
)

target_compile_features(
  ${PROJECT_NAME}
  PUBLIC
    c_std_11
)

set_default_warnings(${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

link_default_libraries(${PROJECT_NAME})
