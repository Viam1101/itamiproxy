#pragma once
#include <stdint.h>
#include "variant.hh"

#define MALLOC(type, ...) (type*)(malloc(sizeof(type) __VA_ARGS__))
#define get_packet_type(packet) (packet->dataLength > 3 ? *packet->data : 0)
#define DO_ONCE            \
    ([]() {                \
        static char o = 0; \
        return !o && ++o;  \
    }())
#ifdef _WIN32
#define INLINE __forceinline
#else //for gcc/clang
#define INLINE inline
#endif

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin() + (*s.data() == '-' ? 1 : 0), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}