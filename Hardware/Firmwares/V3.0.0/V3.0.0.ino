#include "fabgl.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>  // v6.18.0
#include <FS.h>
#include "SPIFFS.h"



fabgl::VGA16Controller VGAController;
fabgl::Canvas cv(&VGAController);
HTTPClient http;

const char* SSID = "e-labinnovations";                 // Your WiFi SSID
const char* PASSWORD = "PASSWORD";  // Your WiFi Password

const char* Server_URL = "http://scnotifier.herokuapp.com/img";


void setupVGA() {
  VGAController.begin(GPIO_NUM_12, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_26, GPIO_NUM_25);
  VGAController.setResolution(VGA_640x480_60Hz);

  cv.setBrushColor(RGB888(64, 64, 64));
  cv.setPenColor(RGB888(0, 0, 0));
  cv.clear();

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(vga_output);
}

void setupWiFi() {
  Serial.print("\nConnecting...");
  cv.drawText(5, 0, "Connecting...");
  /*
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.print("\nConnecting...");
  */

  WiFi.begin(SSID, PASSWORD);
  int x = 200;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    cv.drawText(x, 5, ".");
    x += 5;
    delay(500);
  }
  Serial.print("\nConnected - ");
  Serial.println(WiFi.localIP());
  cv.drawText(5, 20, "Connected - ");
//  cv.drawText(50, 20, WiFi.localIP());
  //  tft.print("\nConnected - ");
  //  tft.println(WiFi.localIP());
}


bool vga_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if ( y >= cv.getHeight() ) return 0;

  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  return 1;
}

void loadFile() {
  TJpgDec.drawFsJpg(0, 0, "/img.jpg");
}

void downloadImage(String imgURL) {
  Serial.println(imgURL);
  File f = SPIFFS.open("/img.jpg", "w");
  if (f) {
    http.begin(imgURL);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      http.writeToStream(&f);
      loadFile();
    } else {
      Serial.printf("Getting images failed, code:%d, error: %s\n", httpCode, http.errorToString(httpCode).c_str());
    }
    f.close();
  }
  http.end();
}

void showData() {
  downloadImage(Server_URL);
  delay(5000);
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }
  Serial.println("SPIFFS Initialisation done.");

  setupVGA();
  setupWiFi();
}

void loop() {
  showData();
}


void writePixel(int16_t x, int16_t y, uint16_t rgb565) {
  uint8_t r = ((rgb565 >> 11) & 0x1F) * 255 / 31; // red   0.0 .. 255
  uint8_t g = ((rgb565 >> 5) & 0x3F) * 255 / 63.0;  // green 0.0 .. 255
  uint8_t b = (rgb565 & 0x1F) * 255 / 31.0;         // blue  0.0 .. 255
  cv.setPixel(x, y, RGB888(r, g, b));
}
