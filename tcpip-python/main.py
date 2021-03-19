import platform
import threading
from definitions import AnsiColors


def main():
    print(f"{AnsiColors.RESET}Starting TCP/IP Server/Client Python demo on {platform.system()}")
    server_thread = threading.Thread(target=server_function, args=(None,))
    client_thread = threading.Thread(target=client_function, args=(None,))
    server_thread.start()
    client_thread.start()
    server_thread.join()
    client_thread.join()
    print(f"{AnsiColors.RESET}Demo finished.")


def server_function(args: any):
    pass


def client_function(args: any):
    pass


if __name__ == "__main__":
    main()
