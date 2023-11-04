#include <Arduino.h>
#include <TFT_eSPI.h>

void* my_malloc(const char* what, std::size_t size) {
    (void) what;
    Serial.print("malloc ");
    Serial.print(what);
    Serial.print(":  ");
    Serial.println(size);
    return malloc(size);
}

void* my_realloc(const char* what, void* ptr, std::size_t new_size) {
    (void) what;
    Serial.print("realloc ");
    Serial.print(what);
    Serial.print(":  ");
    Serial.println(new_size);
    return realloc(ptr, new_size);
}

#define PL_MPEG_IMPLEMENTATION
#define PLM_BUFFER_DEFAULT_SIZE (32 * 1024)
#define PLM_MALLOC(what, sz) my_malloc(what, sz)
#define PLM_FREE(p) free(p)
#define PLM_REALLOC(what, p, sz) my_realloc(what, p, sz)
#include "pl_mpeg.h"

#include "MPEG1Video.h"
#define VIDEO_DATA full_mpg
#define VIDEO_LEN  full_mpg_len

constexpr int WIDTH = 240;
constexpr int HEIGHT = 240;

TFT_eSPI tft = TFT_eSPI();

inline int16_t convertPixel(int8_t luma) {
    // return tft.color565(luma / 8, luma, luma / 8);
    return tft.color565(0, luma, 0);
}

void show_frame(plm_frame_t *frame) {
    tft.startWrite();
    tft.setAddrWindow(0, 0, WIDTH, HEIGHT);
    int numPixels = WIDTH * HEIGHT;
    int i = 0;
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            uint32_t luma = frame->y.data[++i];
            tft.pushColor(convertPixel(luma));
        }
    }
    tft.endWrite();
}

void play_video() {
    plm_t *plm =
        plm_create_with_memory(
                const_cast<uint8_t*>(VIDEO_DATA),
                VIDEO_LEN,
                false, true);

    plm_set_audio_enabled(plm, false);
    plm_set_loop(plm, true);

    int frame_count = 0;
    auto last_time = millis();

    // Decode forever until power is removed
    while (true) {
        auto *frame = plm_decode_video(plm);
        show_frame(frame);

        if (++frame_count == 30) {
            frame_count = 0;
            auto now = millis();
            Serial.println(now - last_time);
            last_time = now;
        }
    }
}

void setup() {
    Serial.begin(19200);
    // while (!Serial) {}
    Serial.println("starting on Pico");
    tft.init();
    tft.fillScreen(TFT_BLACK);
    play_video();
}

void loop() {
}

// TODO: figure out why loop fails
// TODO: figure out frame corruption