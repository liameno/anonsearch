cmake_minimum_required(VERSION 3.20)
project(backend)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL)
find_package(Threads)

add_executable(${PROJECT_NAME} main.cpp config.h third_party/httplib.h helper.cpp helper.h)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads curl /usr/local/lib/libanonsearch.so)