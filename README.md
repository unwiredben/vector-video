# vector-video

This is my badge hacking project for Supercon 2023.  I wanted to make video run on the badge's 1.8" round LCD screen, but hit some interesting obstacles.

This is a complete takeover and doesn't currently use the ADC/DAC component, although getting audio working is a stretch goal.

Since the screen has a 240x240 addressable area, the MPEG-1 files are about twice the size of my earlier badger-movie project which renders MPEG-1 to a 226x128 ePaper display.
When I tried to use my code, I quickly ran into memory allocation failures, as there's not enough room in the Pico's 208K of memory to allocate three reference frames.
After looking for optimizations, I decided that I needed to modify the library to run in "luma-only" mode, where only the Y plane was being decoded.  I got this working fairly
quickly, but had a lingering display bug caused by exiting the Cr/Cb render stage a litle too early that I nailed down on Saturday night.

# Social Media Updates

https://nycr.social/@unwiredben/111356286237886104 has my first public video teaser.

# Links

* https://hackaday.com/2023/10/18/2023-hackaday-supercon-badge-welcome-to-the-vectorscope/
* https://github.com/Hack-a-Day/Vectorscope/blob/main/source/pin_defs.py - pin definitions from the badge's MicroPython implementation
* https://github.com/phoboslab/pl_mpeg - the awesome single file MPEG-1 video decoder that I used
* https://github.com/Bodmer/TFT_eSPI/ - the display library I used. Got plans to contribute back to this one.
* https://github.com/earlephilhower/arduino-pico - a better Pico SDK than the default mBed one
* https://platformio.org/ - the VSCode IDE extension that made building C++ code for the badge relatively easy

# FFMPEG conversion examples

Monochrome

```
ffmpeg -i INPUT.mp4 -vf "crop=in_h" -an -t 00:00:20 -s 240x240 -vf hue=s=0 -vcodec mpeg1video OUTPUT.mpg
```

Color

```
ffmpeg -i INPUT.mp4 -vf "crop=in_h" -an -t 00:00:20 -s 240x240 -vcodec mpeg1video OUTPUT.mpg
```

Color 4:3

```
ffmpeg -i INPUT.mp4 -vf "crop=4*ih/3:ih" -an -t 00:00:20 -s 240x180 -vcodec mpeg1video OUTPUT.mpg
```

Color 4:3 with a little side cropping

```
ffmpeg -i INPUT.mp4 -vf "crop=4*(ih-40)/3:(ih-40)" -an -t 00:00:20 -s 240x180 OUTPUT.mpg
```

In my testing, I usually made smaller MPEG-1 files in color mode; it may be that the
video filtering to remove hue is adding noise to the original video.

I found using ffplay useful in previewing the output, you just need to leave off the output filename.

You can jump-cut MPEG-1 files together just by concatenating them, so you can make a larger video by taking multiple smaller ones.

# FFMPEG to C include

I'm doing this manually using the command

```
xxd -i video.mpg | sed -e "s/unsigned/const unsigned/" > MPEG1Video.h
```

The naming of the data in the header file depends on the input filename, so you may want an addition sed stage
to convert to a generic name to avoid needing to change your player source.
