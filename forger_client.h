#pragma once
#include "enet/include/enet.h"

class forger_client
{
public:
    forger_client();
    ~forger_client();
    void tick();
    void yield();
public:

    ENetHost* enethost;
private:

};

