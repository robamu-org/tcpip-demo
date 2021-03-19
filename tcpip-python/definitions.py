import enum


class AnsiColors:
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    YELLOW = "\x1b[33m"
    BLUE = "\x1b[34m"
    MAGNETA = "\x1b[35m"
    CYAN = "\x1b[36m"
    RESET = "\x1b[0m"


SERVER_ADDRESS = "localhost"
SERVER_PORT = 27015


class Protocols(enum.Enum):
    TCPIP_PROT_0_TCP = 0
    TCPIP_PROT_1_UDP = 1


class Types(enum.Enum):
    TCPIP_TYPE_0_CLIENT_SERVER = 0
    TCPIP_TYPE_1_SERVER_ONLY = 1
    TCPIP_TYPE_2_CLIENT_ONLY = 1
