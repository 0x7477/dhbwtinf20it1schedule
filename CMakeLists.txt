cmake_minimum_required(VERSION 3.0)
project(
dhbwtinf20it1schedule
VERSION 0.1
LANGUAGES CXX
)

message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

set(CMAKE_BUILD_TYPE          Debug)
set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -lcurl")
message(STATUS "Flags are  ${CMAKE_CXX_FLAGS}\n")



set(EXEC schedule)

# main app
file(GLOB SOURCES src/*.cpp)

add_executable(${EXEC} ${SOURCES})
target_link_libraries(${EXEC} curl)
