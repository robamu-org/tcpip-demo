target_sources(${TARGET_NAME} PRIVATE
    client.cpp 
    server.cpp 
)

if(WIN32)
    add_subdirectory(win32)
endif()