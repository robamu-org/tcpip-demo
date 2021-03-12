cmake_minimum_required (VERSION 3.6)

project (tcpip_demo)

# Set names and variables
set(TARGET_NAME ${CMAKE_PROJECT_NAME}) 

# Add executable
add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
	main.cpp
	client.cpp
	server.cpp
)

target_link_libraries(${TARGET_NAME}
	ws2_32
)