cmake_minimum_required(VERSION 3.20)
project(anonsearch)

set(CMAKE_CXX_STANDARD 14)

find_package(CURL)
find_package(Threads)

set(include include/engine.h include/anonsearch.h)
set(include_engines
        include/engines/answers.h
        include/engines/brave.h include/engines/duckduckgo.h
        include/engines/mojeek.h include/engines/yahoo.h
        include/engines/ip_api.h include/engines/wikipedia.h)
set(include_third_party include/third_party/json.hpp)
set(include_third_party_str include/third_party/str/str.h include/third_party/str/str_impl.h)
set(include_third_party_http include/third_party/http/http.h)
set(include_third_party_simple_web include/third_party/simple_web/html.h include/third_party/simple_web/xpath.h)

set(src
        src/engines/engine.cpp src/engines/answers.cpp
        src/engines/brave.cpp src/engines/duckduckgo.cpp
        src/engines/mojeek.cpp src/engines/yahoo.cpp
        src/engines/ip_api.cpp src/engines/wikipedia.cpp
        src/third_party/http/http.cpp src/third_party/simple_web/html.cpp
        src/third_party/simple_web/xpath.cpp
        src/third_party/str/str.cpp)

add_library(${PROJECT_NAME} SHARED ${include} ${src})
target_include_directories(${PROJECT_NAME} PUBLIC include)
include(GNUInstallDirs)

install(FILES ${include} DESTINATION include/${PROJECT_NAME})
install(FILES ${include_engines} DESTINATION include/${PROJECT_NAME}/engines)
install(FILES ${include_third_party} DESTINATION include/${PROJECT_NAME}/third_party)
install(FILES ${include_third_party_str} DESTINATION include/${PROJECT_NAME}/third_party/str)
install(FILES ${include_third_party_http} DESTINATION include/${PROJECT_NAME}/third_party/http)
install(FILES ${include_third_party_simple_web} DESTINATION include/${PROJECT_NAME}/third_party/simple_web)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
