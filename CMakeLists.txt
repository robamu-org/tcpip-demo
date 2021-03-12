cmake_minimum_required (VERSION 3.13)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project (tcpip_demo)

# Set names and variables
set(TARGET_NAME ${CMAKE_PROJECT_NAME}) 

# Add executable
add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
	main.cpp
)

add_subdirectory(oneshot_demo)
add_subdirectory(core)

target_link_libraries(${TARGET_NAME}
	ws2_32
)

# Add include paths for all sources.
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)