# vector-video

This is my badge hacking project for Supercon 2023.  I wanted to make video run on the badge's 1.8" round LCD screen, but hit some interesting obstacles.

This is a complete takeover and doesn't currently use the ADC/DAC component, although getting audio working is a stretch goal.

Since the screen has a 240x240 addressable area, the MPEG-1 files are about twice the size of my earlier badger-movie project which renders MPEG-1 to a 226x128 ePaper display.
When I tried to use my code, I quickly ran into memory allocation failures, as there's not enough room in the Pico's 208K of memory to allocate three reference frames.
After looking for optimizations, I decided that I needed to modify the library to run in "luma-only" mode, where only the Y plane was being decoded.  I got this working fairly
quickly, but had a lingering display bug caused by exiting the Cr/Cb render stage a litle too early that I nailed down on Saturday night.

# Links

* https://hackaday.com/2023/10/18/2023-hackaday-supercon-badge-welcome-to-the-vectorscope/
* https://github.com/Hack-a-Day/Vectorscope/blob/main/source/pin_defs.py - pin definitions from the badge's MicroPython implementation
* https://github.com/phoboslab/pl_mpeg - the awesome single file MPEG-1 video decoder that I used
* https://github.com/Bodmer/TFT_eSPI/ - the display library I used. Got plans to contribute back to this one.
* https://github.com/earlephilhower/arduino-pico - a better Pico SDK than the default mBed one
* https://platformio.org/ - the VSCode IDE extension that made building C++ code for the badge relatively easy
