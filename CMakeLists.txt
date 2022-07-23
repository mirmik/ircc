project(ircc)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(SOURCES 
	src/main.cpp)

add_executable(ircc ${SOURCES})

target_link_libraries(ircc igris)

install(TARGETS ircc DESTINATION /usr/local/bin)