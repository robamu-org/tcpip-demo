#include "TcpipBase.h"

TcpipBase::TcpipBase(tcpip::DemoConfig &cfg, size_t reception_buf_size):
        prot(cfg.prot), type(cfg.type), mode(cfg.mode),
        server_address(cfg.server_address), server_port(cfg.server_port),
        reception_buffer(reception_buf_size) {
}

TcpipBase::~TcpipBase() {
}
