target_sources(${TARGET_NAME} PRIVATE
    client.cpp 
    server.cpp 
    TcpipBase.cpp
    tcpipCommon.cpp
)

add_subdirectory(tcp)