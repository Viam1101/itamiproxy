#pragma once
#include <iostream>
#include "enet/include/enet.h"
#include "proton/variant.hh"
#include "proton/rtparam.hh"
#include <random>
#include <chrono>
#include <mutex>

#define DO_ONCE            \
    ([]() {                \
        static char o = 0; \
        return !o && ++o;  \
    }())

enum {
    PACKET_STATE = 0,
    PACKET_CALL_FUNCTION,
    PACKET_UPDATE_STATUS,
    PACKET_TILE_CHANGE_REQUEST,
    PACKET_SEND_MAP_DATA,
    PACKET_SEND_TILE_UPDATE_DATA,
    PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE,
    PACKET_TILE_ACTIVATE_REQUEST,
    PACKET_TILE_APPLY_DAMAGE,
    PACKET_SEND_INVENTORY_STATE,
    PACKET_ITEM_ACTIVATE_REQUEST,
    PACKET_ITEM_ACTIVATE_OBJECT_REQUEST,
    PACKET_SEND_TILE_TREE_STATE,
    PACKET_MODIFY_ITEM_INVENTORY,
    PACKET_ITEM_CHANGE_OBJECT,
    PACKET_SEND_LOCK,
    PACKET_SEND_ITEM_DATABASE_DATA,
    PACKET_SEND_PARTICLE_EFFECT,
    PACKET_SET_ICON_STATE,
    PACKET_ITEM_EFFECT,
    PACKET_SET_CHARACTER_STATE,
    PACKET_PING_REPLY,
    PACKET_PING_REQUEST,
    PACKET_GOT_PUNCHED,
    PACKET_APP_CHECK_RESPONSE,
    PACKET_APP_INTEGRITY_FAIL,
    PACKET_DISCONNECT,
    PACKET_BATTLE_JOIN,
    PACKET_BATTLE_EVEN,
    PACKET_USE_DOOR,
    PACKET_SEND_PARENTAL,
    PACKET_GONE_FISHIN,
    PACKET_STEAM,
    PACKET_PET_BATTLE,
    PACKET_NPC,
    PACKET_SPECIAL,
    PACKET_SEND_PARTICLE_EFFECT_V2,
    GAME_ACTIVE_ARROW_TO_ITEM,
    GAME_SELECT_TILE_INDEX
};

enum {
    NET_MESSAGE_UNKNOWN = 0,
    NET_MESSAGE_SERVER_HELLO,
    NET_MESSAGE_GENERIC_TEXT,
    NET_MESSAGE_GAME_MESSAGE,
    NET_MESSAGE_GAME_PACKET,
    NET_MESSAGE_ERROR,
    NET_MESSAGE_TRACK,
    NET_MESSAGE_CLIENT_LOG_REQUEST,
    NET_MESSAGE_CLIENT_LOG_RESPONSE,
};

#pragma pack(push, 1) //no padding
struct game_packet {
    uint8_t type; //1
    uint8_t netid; //1
    uint8_t jump_amount; //1
    uint8_t count; //1
    int32_t intval1; //4
    int32_t intval2; //4
    int32_t intval3; //4
    float float1; //4
    int32_t intval4; //4
    float vec_x; //4
    float vec_y; //4
    float vec2_x; //4
    float vec2_y; //4
    float float2; //4
    uint32_t state1; //4
    uint32_t state2; //4
    uint32_t data_size; //4
    uint32_t data; //4
    // -- ??
    // 60 in total
};
//Thx to Ama6nen/Amateurz (Amaproxy)

struct extended_packet {
    int32_t type;
    char data;
};
#pragma pack(pop) //back

class Global {
public:
    //Utils
    static bool regex_find(std::string &txt, const char* keyword);
    static std::mt19937 rng;
    static int random(int min, int max) {
        if (DO_ONCE)
            rng.seed((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(rng);
    }
    
    static std::string hex_str(unsigned char data) {
        const char hexmap_s[17] = "0123456789abcdef";
        std::string s(2, ' ');
        s[0] = hexmap_s[(data & 0xF0) >> 4];
        s[1] = hexmap_s[data & 0x0F];
        return s;
    }
    static std::string generate_mac(const std::string& prefix) {
        std::string x = prefix + ":";
        for (int i = 0; i < 5; i++) {
            x += hex_str(random(0, 255));
            if (i != 4)
                x += ":";
        }
        return x;
    }
    static std::string generate_rid() {
        std::string rid_str;

        for (int i = 0; i < 16; i++)
            rid_str += hex_str(random(0, 255));

        std::transform(rid_str.begin(), rid_str.end(), rid_str.begin(), ::toupper);

        return rid_str;
    }

public:

    static void dc_client(bool local_server);
    static game_packet* split_packet(ENetPacket* packet);
    static void send_varlist(bool gt_peer, variantlist_t& list, int32_t netid = -1, int32_t delay = 0);
    static void send_game(bool gt_peer, int32_t type, uint8_t* data, int32_t len);
    static void send_text(bool gt_peer, std::string text, int32_t type = 2);
    static void action_log(std::string text);

public:
    static std::atomic_bool stop_event;
    static std::atomic_bool gtpeer_available;
    static std::atomic_bool ingame;
    static ENetPeer* gt_peer;
    static ENetPeer* ubi_server;
    static std::string server_ip;
    static enet_uint16 server_port;
    static std::string meta;
private:
    //No Constructors needed
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;
};