#pragma once
#include "enet/include/enet.h"

class local_server {
public:
	local_server();
	void tick();
	void onConnect();

public:
	ENetHost* enethost;
private:

};