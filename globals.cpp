#include "globals.h"
#include <thread>
#include <regex>

std::mutex mut_dc;
void Global::dc_client(bool local_server)
{
    std::lock_guard<std::mutex> lock(mut_dc);
    if (Global::gtpeer_available) Global::gtpeer_available = false;
    stop_event = true;
	printf("[Global] Stop event was called, waiting other thread to be done.\n");
	while (local_server && stop_event) {
		std::this_thread::yield();
	}
    if (Global::gt_peer) {
        enet_peer_disconnect_now(Global::gt_peer, 0);
        Global::gt_peer = nullptr;
    }
    Global::ingame = false;
}
bool Global::regex_find(std::string& txt, const char* keyword)
{
    return std::regex_match(txt, std::regex(keyword));
}

game_packet* Global::split_packet(ENetPacket* packet)
{
    if (packet->dataLength < sizeof(game_packet) - 4) //exclude size
        return nullptr;
    
    game_packet* gamepacket = reinterpret_cast<game_packet*>(packet->data + 4); //skip size
    if (gamepacket->intval3 & 8) { //text packet
        if (packet->dataLength < gamepacket->data_size + 60) {
            printf("[Global] Packet is too small to process.\n");
            return nullptr;
        }
        extended_packet* extended = reinterpret_cast<extended_packet*>(packet->data);
        return reinterpret_cast<game_packet*>(&extended->data);
    }
    else
        gamepacket->data_size = 0;
    return gamepacket;
}

std::mutex mut_variant;
void Global::send_varlist(bool gt_peer, variantlist_t& list, int32_t netid, int32_t delay)
{
    if (!Global::gtpeer_available || Global::stop_event) return;
    ENetPeer* peer = (gt_peer ? Global::gt_peer : Global::ubi_server);
    if (!peer || peer->state != ENET_PEER_STATE_CONNECTED) return;

    uint32_t data_size = 0;
    void* data = list.serialize_to_mem(&data_size, nullptr);

    //Credits to Ama6nen
    //I have no idea how i can send them without packing/casting so I'm leaving it here until an idea pops up.

    uint32_t size = sizeof(game_packet) + data_size;
    game_packet* game = new game_packet[size];
    if (!game) return;
    extended_packet* extended = new extended_packet[sizeof(extended_packet) + size];
    if (!extended) return;

    memset(game, 0, size); //cleaning up
    memset(extended, 0, sizeof(extended) + size); //cleaning up
    

    game->type = PACKET_CALL_FUNCTION;
    game->intval1 = netid;
    game->intval3 |= 8;
    game->intval4 = delay;
    memcpy(&game->data, data, data_size);
    game->data_size = data_size;
    memcpy(&extended->data, game, size);
    delete[] game;
    extended->type = NET_MESSAGE_GAME_PACKET;
    ENetPacket* packet = enet_packet_create(extended, size, ENET_PACKET_FLAG_RELIABLE);

    //mutex?
    std::lock_guard<std::mutex> lock(mut_variant);
    if (enet_peer_send(peer, 0, packet) != 0) printf("Error sending packet - send_varlist!\n");
    //!peer || peer->state != ENET_PEER_STATE_CONNECTED - need double check?
    delete[] extended;
}

std::mutex mut_game;
void Global::send_game(bool gt_peer, int32_t type, uint8_t* data, int32_t len)
{
    if (!Global::gtpeer_available || Global::stop_event) return;
    ENetPeer* peer = (gt_peer ? Global::gt_peer : Global::ubi_server);
    if (!peer || peer->state != ENET_PEER_STATE_CONNECTED) return;

    ENetPacket* packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    extended_packet* extended = (extended_packet*)packet->data;
    extended->type = type;
    if (data) memcpy(&extended->data, data, len);

    memset(&extended->data + len, 0, 1);
    std::lock_guard<std::mutex> lock(mut_game);
    if (enet_peer_send(peer, 0, packet) != 0) printf("Error sending packet - send_game!\n");

}

std::mutex mut_text;
void Global::send_text(bool gt_peer, std::string text, int32_t type)
{
    if (!Global::gtpeer_available || Global::stop_event) return;
    ENetPeer* peer = (gt_peer ? Global::gt_peer : Global::ubi_server);
    if (!peer || peer->state != ENET_PEER_STATE_CONNECTED) return;

    ENetPacket* packet = enet_packet_create(0, text.length() + 5, ENET_PACKET_FLAG_RELIABLE);
    extended_packet* extended = reinterpret_cast<extended_packet*>(packet->data);
    extended->type = type;
    memcpy(&extended->data, text.c_str(), text.length());
    memset(&extended->data + text.length(), 0, 1); //cleaning

    std::lock_guard<std::mutex> lock(mut_text);
    if (enet_peer_send(peer, 0, packet) != 0) printf("Error sending packet! - send_text\n");
}

void Global::action_log(std::string text)
{
    send_text(true, "action|log\nmsg|" + text, 3);
}


std::mt19937 Global::rng;
std::string Global::meta = "defined";
std::atomic_bool Global::stop_event = false;
std::atomic_bool Global::ingame = false;
std::atomic_bool Global::gtpeer_available = false;
ENetPeer* Global::gt_peer = nullptr;
ENetPeer* Global::ubi_server = nullptr;
std::string Global::server_ip = "1.1.1.1";
enet_uint16 Global::server_port = 1;