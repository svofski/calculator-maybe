#pragma once

#ifndef ARDUINO

#include <cstdio>
#include <cassert>

#include <conio.h>

struct FakeSerial {
    void begin(int baud) {}

    void println() {
        printf("\n");
    }

    void print(char c) {
        putchar(c);
    }

    void print(unsigned char c) {
        putchar(c);
    }

    void print(signed char c) {
        putchar(c);
    }

    void print(int n, int radix = 10) {
        switch (radix) {
            case 10: printf("%d", n); break;
            case 16: printf("%x", n); break;
            default: assert(false);
        }
    }

    void print(const char * s) {
        printf("%s", s);
    }

    void println(const char * s) {
        print(s);
        println();
    }

    void println(int n, int radix = 10) {
        print(n, radix);
        println();
    }

    int available() {
        return kbhit();
    }

    int read() {
        if (kbhit()) {
            return getche();
        }
        return 0;
    }
};

extern FakeSerial Serial;

#endif