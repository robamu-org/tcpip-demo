
cmake_minimum_required (VERSION 3.13)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(demoConfig.h.in demoConfig.h)

project (tcpip_demo)

# Set names and variables
set(TARGET_NAME ${CMAKE_PROJECT_NAME}) 

# Add executable
add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
	main.cpp
	utility.cpp
)

add_subdirectory(oneshot_demo)
add_subdirectory(core)
add_subdirectory(spdlog)

target_link_libraries(${TARGET_NAME} PRIVATE
    spdlog::spdlog
)
if(WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE
	   ws2_32
    )
elseif(UNIX)
    find_package(Threads REQUIRED)

    target_link_libraries(${TARGET_NAME} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        rt
    )
endif()

# Add include paths for all sources.
target_include_directories(${TARGET_NAME} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include (${CMAKE_CURRENT_SOURCE_DIR}/BuildType.cmake)
set_build_type()