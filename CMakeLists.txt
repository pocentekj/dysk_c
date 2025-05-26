cmake_minimum_required(VERSION 3.10)
project(dysk_c C)

# Set C standard and warning flags
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g")

# Set output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add the executable and its sources
add_executable(dysk_c src/colors.c src/main.c)

# Link additional required libraries
target_link_libraries(dysk_c m)
