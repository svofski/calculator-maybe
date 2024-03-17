#pragma once

#include <Arduino.h>
#include <cstdint>

// VFD emulation for serial terminal
class TermVFD {
    private:
        uint8_t display_chars[2][12];
        uint8_t display_dots[2][12];
        int wrbuf = 0;

        int rdbuf() const { return wrbuf^1; }
    public:
        TermVFD() {}

        void flip_buffers() {
            wrbuf = rdbuf();
        }

        void set_digit(int pos, int digit, int dot)
        {
            static char eggox[] = {'-', 'L', 'C', 'r', 'E', ' '}; // A-F in eggognese: note that C and E match sane hex

            if (pos >= 0 && pos < 12) {
                if (digit >= 0 && digit < 16) {
                    // don't change
                }
                else if (digit >= '0' && digit <= '9') {
                    digit = digit - '0';
                }
                else if (digit >= 'A' && digit <= 'F') {
                    digit = digit - 'A' + 10;
                }
                else {
                    for (int i = 0; i < sizeof(eggox); ++i) {
                        if (digit == eggox[i]) {
                            digit = i + 10;
                            break;
                        }
                    }
                }

                display_chars[wrbuf][pos] = digit;
                display_dots[wrbuf][pos] = dot != ' ' && dot != 0;
            }
        }

        void print_display()
        {
            static const char segments[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'L', 'C', 'r', 'E', ' '};
            static const char dotses[2] = {' ', ','};

            bool changed = false;
            for (int i = 0; i < 12; ++i) {
                changed = display_chars[rdbuf()][i] != display_chars[wrbuf][i];
                changed |= display_dots[rdbuf()][i] != display_dots[wrbuf][i];
                if (changed) break;
            }

            if (!changed) {
                return;
            }

            Serial.print('[');
            for (int i = 0; i < 12; ++i) {
                Serial.print((char)segments[display_chars[rdbuf()][i]]);
                Serial.print((char)dotses[display_dots[rdbuf()][i]]);
            }
            Serial.print("]  ");

            Serial.print('\r');
        }

};

