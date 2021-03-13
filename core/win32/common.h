#ifndef CORE_WIN32_COMMON_H_
#define CORE_WIN32_COMMON_H_

namespace tcpip {

enum Modes {
    //! Simple demo, but executed by class
    CLIENT_ONE_PACKET_SERVER_ECHO,
    CLIENT_NO_DATA_SERVER_REPLY,
    CLIENT_MUTLIPLE_DATA_SERVER_NO_REPLY,
    CLIENT_MULTIPLE_DATA_SERVER_MULTIPLE_REPLY
};

}

#endif /* CORE_WIN32_COMMON_H_ */
