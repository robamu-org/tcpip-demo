from socket import socket, AF_INET, SOCK_DGRAM, SOCK_STREAM, SHUT_WR
from definitions import SERVER_ADDRESS, SERVER_PORT, AnsiColors
from config import BUFFER_SIZES
from utility import print_protected

ERR_IDX = 0


def tcp_client():
    conn_socket = socket(AF_INET, SOCK_STREAM)
    target_address = SERVER_ADDRESS, SERVER_PORT
    string = "Hello, this is a UDP test!"
    data = string.encode(encoding='utf-8')
    data = string.encode(encoding='utf-8')
    conn_socket.connect(target_address)
    bytes_sent = conn_socket.sendto(data, target_address)
    print_protected(f"{AnsiColors.CYAN}Client: Sent {bytes_sent} bytes to server")
    conn_socket.shutdown(SHUT_WR)
    bytes_rcvd = conn_socket.recv(BUFFER_SIZES)
    print_protected(f"{AnsiColors.CYAN}Client: Received back {len(bytes_rcvd)} bytes: {bytes_rcvd}")


def udp_client():
    global ERR_IDX
    server_socket = socket(AF_INET, SOCK_DGRAM)
    target_address = SERVER_ADDRESS, SERVER_PORT
    string = "Hello, this is a UDP test!"
    data = string.encode(encoding='utf-8')
    bytes_sent = server_socket.sendto(data, target_address)
    print_protected(f"{AnsiColors.CYAN}Client: Sent {bytes_sent} bytes to server")

    try:
        udp_listen_for_reply(server_socket=server_socket)
    except ConnectionResetError:
        # This can happen if the delay between server start and client start is not large enough
        print("Connection reset error, trying again..")
        server_socket.close()
        if ERR_IDX == 3:
            return 0
        else:
            ERR_IDX += 1
        # Try again
        udp_client()
    server_socket.close()


def udp_listen_for_reply(server_socket: socket):
    reply, from_addr = server_socket.recvfrom(BUFFER_SIZES)
    print_protected(f"{AnsiColors.CYAN}Client: Received back {len(reply)} bytes: {reply}")
