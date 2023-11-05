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

#include "st_intro_color.h"
#include "rickroll.h"

constexpr int WIDTH = 240;
constexpr int HEIGHT = 240;

constexpr int MS_PER_FRAME = 33;

constexpr int USER_BUTTON = 19;

TFT_eSPI tft = TFT_eSPI();

int current_shift_mode = -1;
int red_shift_down, blue_shift_down, green_shift_down;
int red_shift_up, blue_shift_up, green_shift_up;

#define NORMAL_MODES 10
#define RAINBOW_MODE 9
#define PRIDE_START 10
int shift_modes[][6] = {
    { 0, 0, 0, 0, 0, 0 }, // gray
    { 8, 0, 8, 0, 0, 0 }, // green
    { 0, 8, 8, 0, 0, 0 }, // red
    { 8, 8, 0, 0, 0, 0 }, // blue
    { 8, 0, 0, 0, 0, 0 }, // cyan
    { 0, 0, 8, 0, 0, 0 }, // yellow
    { 0, 8, 0, 0, 0, 0 }, // magenta
    { 7, 7, 7, 7, 7, 7 }, // b&w
    { 6, 6, 6, 6, 6, 6 }, // poster
    { 0, 0, 0, 0, 0, 0 }, // RAINBOW MODE
    { 0, 8, 8, 0, 0, 0 }, // PRIDE FLAG STRIPES - RED
    { 2, 2, 8, 0, 0, 8 }, // PRIDE FLAG STRIPES - ORANGE
    { 0, 0, 8, 0, 0, 0 }, // PRIDE FLAG STRIPES - YELLOW
    { 8, 0, 8, 0, 0, 0 }, // PRIDE FLAG STRIPES - GREEN
    { 8, 8, 0, 0, 0, 0 }, // PRIDE FLAG STRIPES - BLUE
    { 4, 8, 4, 4, 0, 4 }, // PRIDE FLAG STRIPES - VIOLET
};

void set_shift_mode(int index) {
    red_shift_down   = shift_modes[index][0];
    green_shift_down = shift_modes[index][1];
    blue_shift_down  = shift_modes[index][2];
    red_shift_up     = shift_modes[index][3];
    green_shift_up   = shift_modes[index][4];
    blue_shift_up    = shift_modes[index][5];
}

void next_shift_mode() {
    current_shift_mode = (current_shift_mode + 1) % NORMAL_MODES;
    set_shift_mode(current_shift_mode);
}

inline int16_t convertPixel(int16_t luma) {
    return tft.color565(
            (luma >> red_shift_down) << red_shift_up,
            (luma >> green_shift_down) << green_shift_up,
            (luma >> blue_shift_down) << blue_shift_up);
}

void show_frame(plm_frame_t *frame) {
    bool rainbowMode = current_shift_mode == RAINBOW_MODE;
    tft.startWrite();
    tft.setAddrWindow(0, 0, WIDTH, HEIGHT);
    int numPixels = WIDTH * HEIGHT;
    int i = 0;
    for (int y = 0; y < HEIGHT; ++y) {
        if (rainbowMode)
            set_shift_mode((y / 40) + PRIDE_START);
        for (int x = 0; x < WIDTH; ++x) {
            uint32_t luma = frame->y.data[++i];
            tft.pushColor(convertPixel(luma));
        }
    }
    tft.endWrite();
}

void play_video(uint8_t const* data, size_t len, bool loop) {
    plm_t *plm =
        plm_create_with_memory(
                const_cast<uint8_t*>(data),
                len,
                false, true);

    plm_set_audio_enabled(plm, false);
    plm_set_loop(plm, loop);

    int frame_count = 0;
    auto last_time = millis();

    // Decode forever until power is removed
    while (true) {
        // pause when user button held
        if (digitalRead(USER_BUTTON) == false) {
            next_shift_mode();
            while (digitalRead(USER_BUTTON) == false) {}
        }
        auto *frame = plm_decode_video(plm);
        if (frame) show_frame(frame);

        auto now = millis();
        if (now - last_time < MS_PER_FRAME) {
            delay(MS_PER_FRAME - (now - last_time));
        }
        last_time = now;

        // if not looping, NULL frame means end
        if (!loop && !frame) break;
    }
    plm_destroy(plm);
}

void show_static_frame() {
    bool rainbowMode = current_shift_mode == RAINBOW_MODE;
    tft.startWrite();
    tft.setAddrWindow(0, 0, WIDTH, HEIGHT);
    int numPixels = WIDTH * HEIGHT;
    int i = 0;
    for (int y = 0; y < HEIGHT; ++y) {
        if (rainbowMode)
            set_shift_mode((y / 40) + PRIDE_START);
        for (int x = 0; x < WIDTH; ++x) {
            uint32_t luma = random(256);
            tft.pushColor(convertPixel(luma));
        }
    }
    tft.endWrite();
}

void play_static(int msDuration) {
    auto now = millis();
    auto last_time = now;
    auto endTime = now + msDuration;
    while ((now = millis()) < endTime) {
        show_static_frame();
        if (now - last_time < MS_PER_FRAME) {
            delay(MS_PER_FRAME - (now - last_time));
        }
        last_time = now;
    }
}

void setup() {
    Serial.begin(19200);
    // while (!Serial) {}
    Serial.println("starting on Pico");
    tft.init();
    tft.fillScreen(TFT_BLACK);
    next_shift_mode(); // set to gray to start
}

void loop() {
    play_static(500);
    play_video(st_intro_color_mpg, st_intro_color_mpg_len, false);
    play_static(500);
    play_video(rickroll_mpg, rickroll_mpg_len, false);
}

