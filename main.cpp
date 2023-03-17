#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")

#include <winsock2.h>
#include <windows.h>
#include "local_server.h"
#include "web.h"
#include "globals.h"

BOOL WINAPI sig_func(DWORD dwCtrlType)
{
    std::ofstream file("C:\\Windows\\System32\\drivers\\etc\\hosts");
    if (file.is_open()) {
        file << "";
        file.close();
    }
    return false;
}

int main()
{
    SetConsoleCtrlHandler(sig_func, true);
    {
        std::ofstream file("C:\\Windows\\System32\\drivers\\etc\\hosts");
        if (file.is_open()) {
            file << "127.0.0.1 www.growtopia1.com\n";
            file.close();
        }
    }
    web webserver;
    local_server* server = new local_server();
    while (server->enethost)
    {
        server->tick();
        if (!Global::gtpeer_available) Sleep(1);
    }
    printf("[Local Server] Program ended due to enet host being nullptr.\n");
    delete server;
    system("pause");
    return 0;
}
