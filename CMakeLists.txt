cmake_minimum_required (VERSION 3.6)

project (hello_world)

# Set names and variables
set(TARGET_NAME ${CMAKE_PROJECT_NAME}) 

# Add executable
add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
	main.cpp
)