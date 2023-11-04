#include <Arduino.h>
#include <TFT_eSPI.h>

void* my_malloc(const char* what, std::size_t size) {
    Serial.print("malloc ");
    Serial.print(what);
    Serial.print(":  ");
    Serial.println(size);
    return malloc(size);
}

void* my_realloc(const char* what, void* ptr, std::size_t new_size) {
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
#define VIDEO_DATA st_intro_color_mpg
#define VIDEO_LEN  st_intro_color_mpg_len

constexpr int WIDTH = 240;
constexpr int HEIGHT = 240;

TFT_eSPI tft = TFT_eSPI();

void show_frame(plm_frame_t *frame) {
    static int frame_count = 0;
    if (++frame_count == 30) {
        frame_count = 0;
        Serial.println(millis());
    }
    tft.setWindow(0, 0, 239, 239);
    int numPixels = WIDTH * HEIGHT;
    for (int i = 0; i < numPixels; ++i) {
        uint32_t luma = frame->y.data[i];
        tft.pushColor(tft.color565(luma, luma, luma));
    }
}

void play_video() {
    plm_t *plm =
        plm_create_with_memory(
                const_cast<uint8_t*>(VIDEO_DATA),
                VIDEO_LEN,
                false, true);

    plm_set_audio_enabled(plm, false);
    plm_set_loop(plm, true);

    // Decode forever until power is removed
    while (true) {
        auto *frame = plm_decode_video(plm);
        show_frame(frame);
    }
}

void setup() {
    Serial.begin(19200);
    while (!Serial) {}
    Serial.println("starting on Pico");
    tft.init();
    tft.fillScreen(TFT_BLACK);
    play_video();
}

void loop() {
}