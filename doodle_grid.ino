#include <WiFi.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <WiFiUdp.h>

#include "led_strip.h"
#include "fonts.h"
//#include "hash.h"

#define WIFI_SSID "twitter @vito_lbs"

#define UDP_LISTEN_PORT 27420
#define UDP_PACKET_SIZE_MAX 512 // x, y, r, g, b

#define I2C_DISPLAY_ADDRESS 0x3c
#define I2C_DISPLAY_SDA 21
#define I2C_DISPLAY_SDC 22

#define LED_BRIGHTNESS_SHIFT_R 3
#define LED_BRIGHTNESS_SHIFT_G 3
#define LED_BRIGHTNESS_SHIFT_B 2

#define LED_COUNT (16*16)

SSD1306Wire display(I2C_DISPLAY_ADDRESS,
                    I2C_DISPLAY_SDA,
                    I2C_DISPLAY_SDC);

static struct led_color_t led_strip_buf_1[LED_COUNT];
static struct led_color_t led_strip_buf_2[LED_COUNT];

struct led_strip_t led_strip = {
  rgb_led_type: RGB_LED_TYPE_WS2812,
  led_strip_length: LED_COUNT,
  rmt_channel: RMT_CHANNEL_1,
  rmt_interrupt_num: 19,
  gpio: GPIO_NUM_19,
  showing_buf_1: 0,
  led_strip_buf_1: led_strip_buf_1,
  led_strip_buf_2: led_strip_buf_2,
  access_semaphore: NULL
};

bool led_init_ok = 0;

//OLEDDisplayUi ui(&display);

//CRGB leds[LED_COUNT];

struct led_color_t black = {
  .red = 0,
  .green = 0,
  .blue = 0
};

WiFiUDP udp;
byte udp_buf[UDP_PACKET_SIZE_MAX];

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

uint32_t packet_count = 0;
uint16_t loop_count = 0;

void debug_write(char* text);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("booting doodle_grid");

  for(int i = 0; i < LED_COUNT; i++) {
    led_strip_buf_1[i] = black;
    led_strip_buf_2[i] = black;
  }

  led_strip.access_semaphore = xSemaphoreCreateBinary();
  led_init_ok = led_strip_init(&led_strip);
  led_strip_show(&led_strip);

  Serial.println("LEDs initialized\n");

  display.init();
  display.clear();
  display.display();

  display.setFont(DejaVu_Sans_Mono_6);

  Serial.println("display initialized\n");

  debug_write("trying ssid ");
  debug_write(WIFI_SSID);
  debug_write("\n");

  WiFi.softAP(WIFI_SSID);

  /* int counter = 0; */
  /* while (WiFi.status() != WL_CONNECTED) { */
  /*   delay(500); */
  /*   Serial.print("."); */
  /*   display.drawString(cursor_x, cursor_y, " "); */
  /*   display.drawString(cursor_x, cursor_y, get_pip(counter)); */
  /*   display.display(); */
  /*   counter++; */
  /* } */
  /* display.drawString(cursor_x, cursor_y, " "); */

  /* debug_write("connected to wifi\n"); */
  display.clear();
  cursor_y = 0;
  debug_write(WiFi.softAPIP().toString().c_str());
  debug_write("\n");

  udp.begin(UDP_LISTEN_PORT);
}

void loop() {
  int packet_size = udp.parsePacket();

  if (6 > packet_size) {
    delay(100);
    return;
  }

  packet_count++;

  udp.read(udp_buf, UDP_PACKET_SIZE_MAX);
  int count = udp_buf[0];
  Serial.println(count);
  for (int i = 0; i < count; i++) {
    int cur_start =
      1 + // count
      (5 * i) // position
      ;

    int cur_end = cur_start + 4;

    if (cur_end > packet_size) {
      Serial.println("got count bigger than buf");
      return;
    }

    byte* cur_buf = udp_buf + cur_start;

    int dest_x = cur_buf[0];
    int dest_y = cur_buf[1];
    int r = cur_buf[2] >> LED_BRIGHTNESS_SHIFT_R;
    int g = cur_buf[3] >> LED_BRIGHTNESS_SHIFT_G;
    int b = cur_buf[4] >> LED_BRIGHTNESS_SHIFT_B;

    if (1 == (dest_x % 2)) {
      dest_y = 15 - dest_y;
    }

    int led_idx = ((dest_x * 16) + dest_y) % LED_COUNT;

    led_strip_set_pixel_rgb(&led_strip,
                            led_idx,
                            r,
                            g,
                            b);

    led_strip_show(&led_strip);
    led_strip_set_pixel_rgb(&led_strip,
                            led_idx,
                            r,
                            g,
                            b);
  }
  led_strip_show(&led_strip);
}

void debug_write(const char* text) {
  size_t len = strlen(text);
  bool newline = '\n' == text[len - 1];
  Serial.print(text);
  display.drawString(cursor_x, cursor_y, text);
  if (newline) {
    cursor_y += 12;
    cursor_x = 0;
  } else {
    cursor_x += len * 5;
  }
  display.display();
}

char* get_pip(int counter) {
    switch (counter % 4) {
      case 0:
        return "-";
      case 1:
        return "\\";
      case 2:
        return "|";
      case 3:
        return "/";
    }
}
