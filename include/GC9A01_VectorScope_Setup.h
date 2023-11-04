#define USER_SETUP_LOADED
#define USER_SETUP_ID 0xFFFFFFFF
#define GC9A01_DRIVER
#define TFT_HEIGHT 240
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define RP2040_PIO_SPI
#define TFT_MOSI 3
#define TFT_SCLK 2
#define TFT_RST  4
#define TFT_DC   5
#define SPI_FREQUENCY  20000000
#include <TFT_eSPI.h>
