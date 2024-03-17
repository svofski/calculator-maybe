/*

	ILC2-12/8L emulation on 256x64 OLED

	svofski 2024

*/

#pragma once

class ILC2128L 
{
private:
    int pin_cs;
    int pin_dc;
    int pin_rst;

    static constexpr int NBUFFERS = 2;

    char display_chars[NBUFFERS][12];
    char display_dots[NBUFFERS][12];

    int wrbuf = 0;

    int rdbuf() const { return wrbuf^1; }

public:
    ILC2128L(int pin_cs, int pin_dc, int pin_rst);
    void begin();
    void end();
    void set_digit(int pos, int digit, int dot);
    void flip_buffers();
    void refresh();
};