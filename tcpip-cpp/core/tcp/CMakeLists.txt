target_sources(${TARGET_NAME} PRIVATE
    TcpClientCommon.cpp
)

if(WIN32)
    add_subdirectory(win32)
elseif(UNIX)
    add_subdirectory(unix)
endif()
