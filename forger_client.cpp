#include "forger_client.h"
#include "globals.h"
#include <thread>

forger_client::forger_client()
{
    enethost = enet_host_create(0, 1, 1, 0, 0);
    enethost->usingNewPacket = true;
    if (!enethost) {
        printf("[Forger Client] Error - Couldn't create ENet Host\n");
        system("pause");
        exit(-1);
    }
    enethost->checksum = enet_crc32;
    if (enet_host_compress_with_range_coder(enethost) != 0) {
        printf("[Forger Client] Error - ENet Host Compress\n");
        system("pause");
        exit(-1);
    }

    ENetAddress address;
    enet_address_set_host(&address, Global::server_ip.c_str());
    address.port = Global::server_port;

    Global::ubi_server = enet_host_connect(enethost, &address, 1, 0);
    
	
}

forger_client::~forger_client()
{
    printf("[Forger Client] Destructor called.\n");
    enet_host_destroy(this->enethost);
    Global::stop_event = false;
}


void forger_client::tick() {
    ENetEvent evt;
    while (enet_host_service(enethost, &evt, 1) > 0) {
        switch (evt.type) {
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            printf("[Forger Client] Disconnected\n");
            this->yield();
            return;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            uint8_t type = 0;
            if (evt.packet->dataLength > 3u) type = *(evt.packet->data);

            switch (type)
            {
            case NET_MESSAGE_TRACK:
            {
                uint8_t zero = 0;
                memcpy(evt.packet->data + evt.packet->dataLength - 1, &zero, 1);
                std::string txt = (char*)(evt.packet->data + 4);
                std::cout << "[Forger Client] - NET_MESSAGE_TRACK: \n" << txt << std::endl;
                break;
            }
            case NET_MESSAGE_CLIENT_LOG_REQUEST:
            {
                std::cout << "[Forger Client] - NET_MESSAGE_CLIENT_LOG_REQUEST\n";
                enet_packet_destroy(evt.packet);
                return;
            }
            case NET_MESSAGE_GENERIC_TEXT:
            {
                uint8_t zero = 0;
                memcpy(evt.packet->data + evt.packet->dataLength - 1, &zero, 1);
                std::string txt = (char*)(evt.packet->data + 4);
                std::cout << "[Forger Client] - NET_MESSAGE_GENERIC_TEXT: \n" << txt << std::endl;
                break;
            }
            case NET_MESSAGE_GAME_MESSAGE:
            {
                uint8_t zero = 0;
                memcpy(evt.packet->data + evt.packet->dataLength - 1, &zero, 1);
                std::string txt = (char*)(evt.packet->data + 4);
                std::cout << "[Forger Client] - NET_MESSAGE_GAME_MESSAGE: \n" << txt << std::endl;
                break;
            }
            case NET_MESSAGE_GAME_PACKET:
            {
                game_packet* packet = Global::split_packet(evt.packet);
                if (!packet) break;

                switch (packet->type)
                {
                case PACKET_PING_REQUEST:
                {
                    game_packet pkt;
                    pkt.type = PACKET_PING_REPLY;
                    pkt.intval4 = packet->intval4;
                    pkt.vec_x = 64.f;
                    pkt.vec_y = 64.f;
                    pkt.vec2_x = 1000.f;
                    pkt.vec2_y = 250.f;
                    Global::send_game(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&pkt, sizeof(game_packet));
                    printf("[Forger Client] GAME_PACKET_PING_REQUEST\n");
                    break;
                }
                case PACKET_CALL_FUNCTION:
                {
                    variantlist_t varlist{};
                    varlist.serialize_from_mem(reinterpret_cast<uint8_t*>(&packet->data_size) + 4);
                    std::string onwhat = varlist[0].get_string();
                    if (onwhat != "OnChangeSkin" && varlist[1].get_string() != "audio/punch_locked.wav") std::cout << "[Forger Client] PACKET_CALL_FUNCTION: " << varlist.print() << std::endl;
                    if (onwhat == "OnSuperMainStartAcceptLogon") Global::ingame = true;
                    else if (onwhat == "OnSendToServer")
                    {
                        Global::action_log("`9Changing Sub-Server...``");

                        Global::server_port = varlist[1].get_uint32();
                        std::string str = varlist[4].get_string();

                        std::string doorid = str.substr(str.find("|"));
                        Global::server_ip = str.erase(str.find("|")); //remove | and doorid from end

                        varlist[1] = 1101;
                        varlist[4] = "127.0.0.1" + doorid;

                        Global::send_varlist(true, varlist);
                        if (Global::gt_peer) {
                            enet_host_flush(Global::gt_peer->host);
                            enet_peer_reset(Global::gt_peer);
                        }
                        Global::dc_client(false);
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                    break;
                }
                default: break;
                }
                break;
            }
            default: break;
            }

            if (Global::gtpeer_available && Global::gt_peer && Global::gt_peer->state == ENET_PEER_STATE_CONNECTED) enet_peer_send(Global::gt_peer, 0, evt.packet);
            else {
                enet_packet_destroy(evt.packet);
                this->yield();
                return;
            }
            break;
        }
        case ENET_EVENT_TYPE_CONNECT:
        {
            printf("[Forger Client] Connection Acquired.\n");
            break;
        }
        default: break;
        }
    }
}

void forger_client::yield()
{
    while (!Global::stop_event) {
        std::this_thread::yield();
    }
}
