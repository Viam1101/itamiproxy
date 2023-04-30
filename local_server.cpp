#include "local_server.h"
#include "globals.h"
#include <thread>

local_server::local_server()
{
	enet_initialize();
	atexit(enet_deinitialize);

    ENetAddress address;
    enet_address_set_host(&address, "0.0.0.0");
    address.port = 1101;
    enethost = enet_host_create(&address, 1, 1, 0, 0);

    enethost->usingNewPacket = false;
    if (!enethost) {
        printf("[Local Server] Error - Couldn't start ENet service\n");
        system("pause");
        exit(-1);
    }
    enethost->checksum = enet_crc32;
    if (enet_host_compress_with_range_coder(enethost)) {
        printf("[Local Server] Error - ENet Host Compress\n");
        system("pause");
        exit(-1);
    }

    printf("[Local Server] Created ENet Host.\n");
}

void local_server::tick()
{
    ENetEvent event;
    while (enet_host_service(enethost, &event, 1) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            printf("[Local Server] ENET_EVENT_TYPE_DISCONNECT\n");
            Global::dc_client(true);
            return;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            if (!Global::gtpeer_available) {
                enet_packet_destroy(event.packet);
                break;
            }

            uint8_t type = 0;
            if (event.packet->dataLength > 3u) type = *(event.packet->data);

            switch (type)
            {
            case NET_MESSAGE_GENERIC_TEXT:
            {
                uint8_t zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
                std::string txt = (char*)(event.packet->data + 4);
                if (Global::regex_find(txt, ".*getDRAnimations.*"))
                {
                    enet_packet_destroy(event.packet);
                    return;
                }
                if (txt.find("requestedName|") != -1)
                {

                    auto mac = Global::generate_mac("00");
                    auto wk = Global::generate_rid();
                    //auto rid = Global::generate_rid();
                    //std::string fz = std::to_string(16197536 - Global::random(1, 100));
                    //std::string zf = std::to_string(4184327 - Global::random(1, 100));

                    //std::string hash1 = std::to_string(-731287505 + Global::random(1, 100));
                    //std::string hash2 = std::to_string(-731928004 - Global::random(1, 100));

                    rtvar var = rtvar::parse(txt);
                    if (!var.find("mac")) var.append("mac|" + mac);
                    else var.set("mac", mac);
                    var.set("country", "tr");
                    var.set("wk", wk);
                    //var.set("rid", rid);
                    //var.set("game_version", "4.19");
                    txt = var.serialize();

                    Global::send_text(false, txt);
                    std::cout << "[Spoofed Credentials] \n" << txt << std::endl;
                    enet_packet_destroy(event.packet);
                    return;
                }
                std::cout << "[Local Server] NET_MESSAGE_GENERIC_TEXT: \n" << txt << "\n\n";
                break;
            }
            case NET_MESSAGE_GAME_MESSAGE:
            {
                uint8_t zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);

                std::string txt = (char*)(event.packet->data + 4);
                if (txt.find("action|quit_to_exit") != -1)
                {

                }
                else if (txt.find("action|quit") != -1)
                {
                    enet_peer_disconnect_later(Global::gt_peer, 0);
                    enet_packet_destroy(event.packet);
                    return;
                }
                std::cout << "[Local Server] - NET_MESSAGE_GAME_MESSAGE: \n" << txt << "\n\n";
                break;
            }
            case NET_MESSAGE_GAME_PACKET:
            {
                game_packet* packet = Global::split_packet(event.packet);
                if (!packet) break;

                switch (packet->type)
                {
                case PACKET_PING_REPLY:
                {
                    printf("[Local Server] GAME_PACKET_PING_REPLY\n");
                    //wh1ter0se <3
                    packet->vec2_x = 1000.f;  //gravity
                    packet->vec2_y = 250.f;   //move speed
                    packet->vec_x = 64.f;     //punch range
                    packet->vec_y = 64.f;     //build range
                    packet->jump_amount = 0;  //for example unlim jumps set it to high which causes ban
                    packet->intval1 = 0; //effect flags. good to have as 0 if using mod noclip, or etc.
                    break;
                }
                case PACKET_APP_INTEGRITY_FAIL:
                {
                    printf("[Local Server] GAME_PACKET_APP_INTEGRITY_FAIL\n");
                    if (Global::ingame) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                    break;
                }
                case PACKET_DISCONNECT:
                {
                    enet_packet_destroy(event.packet);
                    return;
                }
                }
                break;
            }
            case NET_MESSAGE_CLIENT_LOG_RESPONSE:
            {
                printf("[Local Server] GAME_NET_MESSAGE_CLIENT_LOG_RESPONSE\n");
                enet_packet_destroy(event.packet);
                return;
            }
            default:break;
            }

            if (!Global::stop_event && Global::ubi_server && Global::ubi_server->state == ENET_PEER_STATE_CONNECTED) enet_peer_send(Global::ubi_server, 0, event.packet);
            else {
                if (Global::gtpeer_available) Global::gtpeer_available = false;
                if (Global::gt_peer) enet_peer_disconnect_later(Global::gt_peer, 0);
                return;
            }

            break;
        }
        case ENET_EVENT_TYPE_CONNECT:
        {
            Global::gtpeer_available = true;
            Global::gt_peer = event.peer;
            printf("[Local Server] ENET_EVENT_TYPE_CONNECT\n");
            Global::gt_peer->hasNoTimeout = 1; //Added this to prevent being timed out while you are afk. - ama proxy has this issue
            std::thread a(&local_server::onConnect, this);
            a.detach();
            return;
        }
        default: break;
        }
    }

}

#include "forger_client.h"
void local_server::onConnect()
{
    printf("[Local Server] onConnect thread is launched.\n");
    forger_client client;

    while (!Global::stop_event)
    {
        client.tick();
    }

    enet_peer_reset(Global::ubi_server);
    Global::ubi_server = nullptr;
}
