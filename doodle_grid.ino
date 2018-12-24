#include <WiFi.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <WiFiUdp.h>

#include "led_strip.h"
#include "fonts.h"
//#include "hash.h"

#define WIFI_SSID "https://hackers.town/@vito"

#define UDP_LISTEN_PORT 27420
#define UDP_PACKET_SIZE 512 // r, g, b

#define I2C_DISPLAY_ADDRESS 0x3c
#define I2C_DISPLAY_SDA 21
#define I2C_DISPLAY_SDC 22


#define LED_BRIGHTNESS_SHIFT_R 3
#define LED_BRIGHTNESS_SHIFT_G 3
#define LED_BRIGHTNESS_SHIFT_B 2

typedef struct {
    byte r : 4;
    byte g : 4;
    byte b : 4;
} packet_color;

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
byte udp_buf[UDP_PACKET_SIZE];
packet_color* udp_packet_buf = (packet_color*) &udp_buf;

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

uint32_t packet_count = 0;
uint16_t loop_count = 0;

void debug_write(const char* text);

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
  Serial.print("sizeof packet_color ");
Serial.println(sizeof(packet_color));
Serial.print("sizeof byte ");
Serial.println(sizeof(byte));
Serial.print("udp_packet_buf / udp_buf");
Serial.print((unsigned int)(void*)(udp_packet_buf), HEX);
Serial.print((unsigned int)(void*)(&udp_buf), HEX);

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

  if (packet_size < UDP_PACKET_SIZE) {
    delay(100);
    return;
  }

  udp.read(udp_buf, UDP_PACKET_SIZE);

  packet_count++;

  Serial.println(packet_size);

    for (int dest_x = 0; dest_x < 16; dest_x++) {
  for (int dest_y = 0; dest_y < 16; dest_y++) {
    int mod_y = dest_y;
      if (1 == (dest_x % 2)) mod_y = 15 - dest_y;
      int cur_start = (16 * dest_x) + (dest_y);
      packet_color cur_color = udp_packet_buf[cur_start];
      int led_idx = ((dest_x * 16) + mod_y);
      
      Serial.print(led_idx);
      Serial.print(" ");
      Serial.print(cur_start);
Serial.print(" ");  
      Serial.print(cur_color.r);
      Serial.print(" ");
      Serial.print(cur_color.g);
      Serial.print(" ");
      Serial.print(cur_color.b);
      Serial.println();

      led_strip_set_pixel_rgb(&led_strip,
                              led_idx,
                              cur_color.r >> LED_BRIGHTNESS_SHIFT_R,
                              cur_color.g >> LED_BRIGHTNESS_SHIFT_G,
                              cur_color.b >> LED_BRIGHTNESS_SHIFT_B);

      // led_strip_show(&led_strip);


      // led_strip_set_pixel_rgb(&led_strip,
      //                         led_idx,
      //                         cur_color.r,
      //                         cur_color.g,
      //                         cur_color.b);
    }
  }
  
  led_strip_show(&led_strip);
  
}

void debug_write(const char* text) {
  size_t len = strlen(text);
  bool newline = '\n' == text[len - 1];
  Serial.print(text);
  display.drawString(cursor_x, cursor_y, text);
  if (newline){ 
    cursor_y += 12;
    cursor_x = 0;
  } else {
    cursor_x += len * 5;
  }
  display.display();
}

const char* get_pip(int counter) {
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

    return "X";
}
