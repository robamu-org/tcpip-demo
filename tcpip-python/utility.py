from threading import Lock

__print_lock = Lock()


def print_protected(string: str):
    """
    Protected printout to avoid tasks being pre-empted during printout process
    :param string:
    :return:
    """
    __print_lock.acquire()
    print(string)
    __print_lock.release()


def lock_print():
    __print_lock.acquire()


def unlock_print():
    __print_lock.release()
