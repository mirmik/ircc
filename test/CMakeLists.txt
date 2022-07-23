project(ircc)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(SOURCES 
	main.cpp
    ircc_resources.gen.cpp)

add_executable(cmake_runtest ${SOURCES})

add_custom_command(OUTPUT ircc_resources.gen.cpp
    COMMAND ircc resources.txt ircc_resources.gen.cpp
)