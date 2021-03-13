#pragma once

#include <demo_config.h>
#include "common.h"

class ClientClass {
public:

    ClientClass(tcpip::DemoModes mode);

    int performOperation();
private:
    tcpip::DemoModes mode;


};
