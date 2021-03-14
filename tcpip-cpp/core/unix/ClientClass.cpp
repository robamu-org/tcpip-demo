#include "../ClientClass.h"

#include <unistd.h>

#include <iostream>

ClientClass::ClientClass(tcpip::DemoConfig& cfg): mode(cfg.mode),
        server_address(cfg.server_address), server_port(cfg.server_port),
        reception_buffer(tcpip::BUFFER_SIZES) {
}

ClientClass::~ClientClass() {
    close(connect_socket);

}

int ClientClass::performOperation() {
    return 0;
}
