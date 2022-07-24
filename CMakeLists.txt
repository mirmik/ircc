project(ircc)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(SOURCES 
	src/main.cpp)

add_executable(ircc ${SOURCES})
set_target_properties(ircc PROPERTIES PUBLIC_HEADER src/ircc.h)

target_link_libraries(ircc igris)

install(TARGETS ircc 
	DESTINATION /usr/local/bin	
    PUBLIC_HEADER DESTINATION "/usr/local/include/ircc"
)
