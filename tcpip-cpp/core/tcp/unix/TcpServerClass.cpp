#include "../TcpServerClass.h"

#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <stdexcept>

int TcpServerClass::perform_simple_echo_op() {
    int retval = 0;
    // Receive until the peer shuts down the connection
    do {
        int send_result;

        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity(), 0);
        if (retval > 0) {
            std::cout << "Server: Bytes Received: " << retval << std::endl;
            size_t bytes_to_sendback = retval;
            // Echo the buffer back to the sender
            send_result = send(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                    bytes_to_sendback, 0);
            if (send_result < 0) {
                std::cerr << "Server: Send failed with error: " << tcpip::get_last_error() <<
                        std::endl;
                return 1;
            }
            std::cout << "Server: Bytes sent: " << send_result << std::endl;
        }
        else if (retval == 0)
            std::cout << "Server: Client closed connection" << std::endl;
        else  {
            std::cerr << "Server: recv failed with error: " << tcpip::get_last_error() << std::endl;
            return 1;
        }

    } while (retval > 0);

    // shutdown the connection since we're done
    retval = shutdown(client_socket, SHUT_SEND);
    if (retval != 0) {
        std::cerr << "shutdown failed with error: " << errno << std::endl;
        return 1;
    }
    return 0;
}
