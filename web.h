#pragma once
#include <iostream>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

class web {
public:
    web();
    ~web();
    void start();
    void send_request();
    std::string dec_base64(const std::string &text);
};