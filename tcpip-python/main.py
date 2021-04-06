import platform
import threading
import time

from server import tcp_server, udp_server
from client import tcp_client, udp_client
from definitions import AnsiColors


def main():
    print(f"{AnsiColors.RESET}Starting TCP/IP Server/Client Python demo on {platform.system()}")
    server_thread = threading.Thread(target=server_function, args=(None,))
    client_thread = threading.Thread(target=client_function, args=(None,))

    server_thread.start()
    time.sleep(0.001)
    client_thread.start()
    server_thread.join()
    client_thread.join()

    print(f"{AnsiColors.RESET}Demo finished.")


def server_function(args: any):
    from config import TCPIP_PROT, TCPIP_TYPE
    from definitions import Types, Protocols
    if TCPIP_TYPE == Types.TCPIP_TYPE_2_CLIENT_ONLY:
        return

    if TCPIP_PROT == Protocols.TCPIP_PROT_0_TCP:
        print("Selected protocol: TCP")
        tcp_server()
    elif TCPIP_PROT == Protocols.TCPIP_PROT_1_UDP:
        print("Selected protocol: UDP")
        udp_server()

    pass


def client_function(args: any):
    from config import TCPIP_PROT, TCPIP_TYPE
    from definitions import Types, Protocols
    if TCPIP_TYPE == Types.TCPIP_TYPE_1_SERVER_ONLY:
        return

    if TCPIP_PROT == Protocols.TCPIP_PROT_0_TCP:
        tcp_client()
    elif TCPIP_PROT == Protocols.TCPIP_PROT_1_UDP:
        udp_client()

    pass


if __name__ == "__main__":
    main()
