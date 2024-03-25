# MK-61 and stuff

This is a collection of mildly cohesive projects related to Soviet programmable RPN calculator emulation, period-accurate displays and their decent modern imitations.

## ИЛЦ2-12/8Л module

[Standalone VFD Module](/vfdmodule)

![assembled module](/vfd/doc/ilc2-module-assembled.jpg)

## OLED imitation

SH1122 256x64 grayscale OLED is used to fake the VFD look. It's not the real thing, but pretty close.

![fake paws](/vfd/doc/oled-fake-paws.gif)

A part of mk61, it is also available as a standalone minimal project/library: [vfd-imitator](https://github.com/svofski/vfd-imitator)

## MK-61

Based on [mk-61 project by sergev](https://github.com/sergev/mk-61) and [emu145 by fixelsan](https://github.com/fixelsan/emu145/). 

Runs on Pi Pico. Eventually wants to become a usable calculator with buttons and stuff.

[Code here](/arduino/mk61vak)
