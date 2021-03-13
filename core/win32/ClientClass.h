#pragma once

#include "common.h"

class ClientClass {
public:



    ClientClass(tcpip::Modes mode);

    int performOperation();
private:
    tcpip::Modes mode;


};
