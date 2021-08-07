#include <WiFi.h>                 //V1.2.7
#include <WebSocketsClient.h>     //V2.3.5
#include <ArduinoJson.h>          //V6.18.0
#include <WiFiClientSecure.h>     //V1.0
#include <WebSocketsClient.h>     //V2.3.5
#include "fabgl.h"                //V1.0.3
#include "images.h"
#include "FranklinGothicMedium.h"
#include "FONT_Helvetica.h"
#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED false

/**** VGA PINS ****/
#define VGA_RED_PIN   GPIO_NUM_12
#define VGA_GREEN_PIN GPIO_NUM_14
#define VGA_BLUE_PIN  GPIO_NUM_27
#define VGA_HSYNC_PIN GPIO_NUM_26
#define VGA_VSYNC_PIN GPIO_NUM_25

/**** INPUTS ****/
#define PREVIOUS_BUTTON 5
#define SLECT_BUTTON    17
#define NEXT_BUTTON     16
#define VGA_IN          13

/**** OUTPUTS ****/
#define RED_LED   33
#define GREEN_LED 23
#define VGA_OUT   32

const char* SSID      = "e-labinnovations";         // WiFi SSID
const char* PASSWORD  = "PASSWORD";                 // WiFi Password
const char* ServerURL = "scnotifier.herokuapp.com"; //Server Address

fabgl::VGA16Controller VGAController;
fabgl::Canvas cv(&VGAController);
WebSocketsClient webSocket;
DynamicJsonDocument doc(2048);

Sprite sprites[1];
Bitmap wifi = Bitmap(98, 75, &wifi_4_data[0], PixelFormat::Mask, Color::Black);
unsigned long previousMillis = 0;
long interval = 10000;
bool first = true;


bool PREVIOUS_BUTTON_past = true;
bool SLECT_BUTTON_past    = true;
bool NEXT_BUTTON_past     = true;
bool VGA_IN_past          = true;

void showNotification(uint8_t * json) {
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    errorStatus(true);
    return;
  }

  errorStatus(false);

  const char* time = doc["time"];
  previousMillis = millis();
  sprites[0].visible = false;
  VGAController.refreshSprites();

  cv.setPenColor(Color::Black);
  cv.setBrushColor(Color::White);
  cv.clear();
  cv.setPenColor(Color::BrightYellow);
  cv.setBrushColor(Color::BrightRed);
  cv.fillRectangle(550, 450, 640, 475);
  cv.drawText(&fabgl::FONT_10x20, 555, 453, time);
  cv.setPenColor(Color::Black);
  cv.setBrushColor(Color::White);

  if (doc["type"] == "NTF") {
    interval = doc["data"]["readingTime"];
    const char* name = doc["data"]["name"];
    const char* notification = doc["data"]["notification"];

    cv.setPenColor(Color::BrightBlue);
    drawText(&fabgl::FONT_10x20, 5, 10, name);
    int X = cv.textExtent(&fabgl::FONT_10x20, name);
    cv.drawLine(5, 30, X, 30);
    cv.setPenColor(Color::Black);
    drawText(&fabgl::FONT_8x14, 5, 50, notification);
  } else if (doc["type"] == "CP") {
    interval = 10000;
    int period = doc["data"]["period"];
    char periodChar[5];
    itoa(period, periodChar, 10);
    const char* subject = doc["data"]["subject"];
    const char* teacher = doc["data"]["teacher"];
    const char* startAt = doc["data"]["startAt"];
    const char* endAt = doc["data"]["endAt"];

    cv.setPenColor(Color::BrightBlue);
    drawText(&fabgl::FONT_10x20, 5, 10, "Current Periode");
    int X = cv.textExtent(&fabgl::FONT_10x20, "Current Periode");
    cv.drawLine(5, 30, X, 30);
    cv.setPenColor(Color::BrightMagenta);
    cv.drawRectangle(40, 50, 600, 300);
    cv.drawLine(40, 100, 600, 100);
    cv.drawLine(40, 150, 600, 150);
    cv.drawLine(40, 200, 600, 200);
    cv.drawLine(40, 250, 600, 250);
    cv.drawLine(300, 50, 300, 300);
    cv.setPenColor(Color::Black);
    drawText(&fabgl::FONT_10x20, 50, 65, "Periode NO");
    drawText(&fabgl::FONT_10x20, 50, 115, "Subject");
    drawText(&fabgl::FONT_10x20, 50, 165, "Teacher");
    drawText(&fabgl::FONT_10x20, 50, 215, "Starting Time");
    drawText(&fabgl::FONT_10x20, 50, 265, "Ending Time");

    drawText(&fabgl::FONT_10x20, 310, 65, periodChar);
    drawText(&fabgl::FONT_10x20, 310, 115, subject);
    drawText(&fabgl::FONT_10x20, 310, 165, teacher);
    drawText(&fabgl::FONT_10x20, 310, 215, startAt);
    drawText(&fabgl::FONT_10x20, 310, 265, endAt);

  } else if (doc["type"] == "WX") {
    interval = 10000;
    const char* location = doc["data"]["currentWeather"]["location"];
    const char* timeStamp = doc["data"]["currentWeather"]["timestamp"];
    const char* phraseValue = doc["data"]["currentWeather"]["phraseValue"];
    const char* tempValue = doc["data"]["currentWeather"]["tempValue"];
    const char* precipValue = doc["data"]["currentWeather"]["precipValue"];
    const char* tempHiLoValue = doc["data"]["currentWeather"]["tempHiLoValue"];
    const char* icon = doc["data"]["currentWeather"]["icon"];

    cv.setBrushColor(Color::Black);
    cv.setPenColor(Color::White);
    cv.clear();
    cv.setPenColor(Color::BrightYellow);
    cv.setBrushColor(Color::BrightRed);
    cv.fillRectangle(550, 450, 640, 475);
    cv.drawText(&fabgl::FONT_10x20, 555, 453, time);

    cv.setBrushColor(Color::Black);
    cv.setPenColor(Color::White);
    drawText(&fabgl::FONT_10x20, 5, 10, "Weather");
    int X = cv.textExtent(&fabgl::FONT_10x20, "Weather");
    cv.drawLine(5, 30, X, 30);
    cv.setPenColor(Color::BrightCyan);
    cv.drawRectangle(5, 50, 635, 180);
    cv.drawRectangle(5, 210, 635, 390);
    //    cv.setBrushColor(Color::White);
    cv.setPenColor(Color::White);
    drawText(&fabgl::FONT_10x20, 10, 60, location);
    drawText(&fabgl::FONT_8x14, 10, 85, timeStamp);
    drawText(&fabgl::FONT_Franklin_Gothic_Medium, 10, 101, tempValue);
    drawText(&fabgl::FONT_8x14, 10, 140, phraseValue);
    drawText(&fabgl::FONT_8x14, 10, 160, precipValue);
    drawText(&fabgl::FONT_Helvetica, 560, 150, tempHiLoValue);
    drawFile(550, 80, icon);

    drawText(&fabgl::FONT_10x20, 10, 220, "Hourly Forecast");
    int xpos[] = {131, 261, 383, 509};
    for (int i = 0; i <= 3; i++) {
      const char* H_timeStamp = doc["data"]["hourlyForcast"][i]["timestamp"];
      const char* H_tempValue = doc["data"]["hourlyForcast"][i]["tempValue"];
      const char* H_precipValue = doc["data"]["hourlyForcast"][i]["precipValue"];
      const char* H_icon = doc["data"]["hourlyForcast"][i]["icon"];

      X = xpos[i] - cv.textExtent(&fabgl::FONT_8x14, H_timeStamp) / 2;
      drawText(&fabgl::FONT_8x14, X, 250, H_timeStamp);
      X = xpos[i] - cv.textExtent(&fabgl::FONT_Helvetica, H_tempValue) / 2;
      drawText(&fabgl::FONT_Helvetica, X, 270, H_tempValue);
      X = xpos[i] - 64 / 2;
      drawFile(X, 280, H_icon);
      X = xpos[i] - cv.textExtent(&fabgl::FONT_8x14, H_precipValue) + 17 / 2;
      cv.drawBitmap(X, 345, &precipIcon);
      drawText(&fabgl::FONT_8x14, X + 17, 345, H_precipValue);
    }
  } else {
    errorStatus(true);
  }

}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      errorStatus(true);
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      errorStatus(false);
      if (first) {
        cv.drawTextFmt(10, 390, "Server [WS] Connected to url: %s%s", ServerURL, payload);
        first = false;
        delay(1000);
      }

      // send message to server when Connected
      if (digitalRead(VGA_IN)) {
        webSocket.sendTXT("{\"type\":\"status_update\", \"vgaIn\":false}");
      } else {
        webSocket.sendTXT("{\"type\":\"status_update\", \"vgaIn\":true}");
      }

      webSocket.sendTXT("{\"type\":\"command\", \"command\":\"connected\"}");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      showNotification(payload);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void setupServer() {
  // server address, port and URL
  webSocket.begin(ServerURL, 80, "/");
  // event handler
  webSocket.onEvent(webSocketEvent);
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  cv.selectFont(&fabgl::FONT_BIGSERIF_8x16);
  cv.setPenColor(RGB888(255, 0, 255));
  cv.drawText(10, 347, "Server");
  cv.drawLine(10, 363, 58, 363);
  cv.setPenColor(RGB888(0, 0, 0));
  cv.selectFont(&fabgl::FONT_8x14);
  cv.drawText(10, 372, "Connecting to the server");
}

void setupWiFi() {
  Serial.print("\nConnecting...");
  cv.selectFont(&fabgl::FONT_BIGSERIF_8x16);
  cv.setPenColor(RGB888(0, 0, 255));
  cv.drawText(10, 190, "WiFi");
  cv.drawLine(10, 206, 42, 206);
  cv.setPenColor(RGB888(0, 0, 0));
  cv.selectFont(&fabgl::FONT_8x14);
  cv.drawTextFmt(10, 215, "Connecting to %s", SSID);


  WiFi.begin(SSID, PASSWORD);
  int pos = 1;
  errorStatus(true);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    showWiFiLoading(pos);
    delay(300);
    pos++;
    if (pos > 4) pos = 1;
  }
  Serial.print("\nConnected");
  cv.drawText(10, 315, "Connected");
  errorStatus(false);
}

void setupVGA() {
  VGAController.begin(VGA_RED_PIN, VGA_BLUE_PIN, VGA_GREEN_PIN, VGA_HSYNC_PIN, VGA_VSYNC_PIN);
  VGAController.setResolution(VGA_640x480_60Hz);

  sprites[0].addBitmap(&wifi);
  sprites[0].visible = false;
  VGAController.setSprites(sprites, 1);

  cv.setBrushColor(RGB888(64, 64, 64));
  cv.setPenColor(RGB888(0, 0, 0));
  cv.clear();
  cv.drawBitmap(170, 5, &logo);
  cv.drawRectangle(5, 60, cv.getWidth() - 5, cv.getHeight() - 5);

  cv.selectFont(&fabgl::FONT_BIGSERIF_8x16);
  cv.setPenColor(RGB888(255, 0, 0));
  cv.drawText(10, 65, "Project By:");
  cv.drawLine(10, 81, 106, 81);
  cv.setPenColor(RGB888(0, 0, 0));
  cv.selectFont(&fabgl::FONT_8x14);
  cv.drawText(10, 90, "Mohammed Ashad MM");
  cv.drawText(10, 107, "Aswin Rosh");
  cv.drawText(10, 124, "Mohammed Rabeegu");
  cv.drawText(10, 141, "Mohammed Afeed");
  cv.drawText(10, 158, "Rabeebu Rahman");
}

void setup() {
  Serial.begin(115200);

  pinMode(PREVIOUS_BUTTON, INPUT_PULLUP);
  pinMode(SLECT_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(VGA_IN, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(VGA_OUT, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(VGA_OUT, LOW);

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    errorStatus(true);
    return;
  }
  setupVGA();
  delay(500);
  setupWiFi();
  delay(500);
  setupServer();
}

void loop() {
  webSocket.loop();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    webSocket.sendTXT("{\"type\":\"command\", \"command\":\"next\"}");

    digitalWrite(RED_LED, !digitalRead(RED_LED));
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));

    Serial.print("INPUTS -> P: ");
    Serial.print(digitalRead(PREVIOUS_BUTTON));
    Serial.print(" S: ");
    Serial.print(digitalRead(SLECT_BUTTON));
    Serial.print(" N: ");
    Serial.print(digitalRead(NEXT_BUTTON));
    Serial.print(" V: ");
    Serial.println(digitalRead(VGA_IN));
  }

  bool PREVIOUS_BUTTON_present  = digitalRead(PREVIOUS_BUTTON);
  bool SLECT_BUTTON_present     = digitalRead(SLECT_BUTTON);
  bool NEXT_BUTTON_present      = digitalRead(NEXT_BUTTON);
  bool VGA_IN_present           = digitalRead(VGA_IN);

  if (!PREVIOUS_BUTTON_present && PREVIOUS_BUTTON_past) {
    webSocket.sendTXT("{\"type\":\"command\", \"command\":\"previous\"}");
  }
  if (!NEXT_BUTTON_present && NEXT_BUTTON_past) {
    webSocket.sendTXT("{\"type\":\"command\", \"command\":\"next\"}");
  }

  digitalWrite(VGA_OUT, !VGA_IN_present);
  if (VGA_IN_present != VGA_IN_past) {
    String vgaStatus = VGA_IN_present ? "false" : "true";
    webSocket.sendTXT("{\"type\":\"status_update\", \"vgaIn\":" + vgaStatus + "}");
  }


  PREVIOUS_BUTTON_past  = PREVIOUS_BUTTON_present;
  NEXT_BUTTON_past      = NEXT_BUTTON_present;
  VGA_IN_past           = VGA_IN_present;
}

void drawText(fabgl::FontInfo const * fontInfo, int X, int Y, char const * text) {
  int minX = 5,
      minY = 5,
      maxX = cv.getWidth() - 5,
      maxY = cv.getHeight() - 5,
      lineSpacing = 3;

  int fontWidth = fontInfo->width;
  for (; *text; ++text, X += fontWidth * 1) {
    if (X >= maxX) {
      X = minX;
      Y += fontInfo->height + lineSpacing;
    }
    if (*text == '\n') {
      X = minX - fontWidth;
      Y += fontInfo->height + lineSpacing;
      continue;
    }
    if (fontInfo->chptr) {
      // variable width
      uint8_t const * chptr = fontInfo->data + fontInfo->chptr[(int)(*text)];
      fontWidth = *chptr++;
      cv.drawGlyph(X, Y, fontWidth, fontInfo->height, chptr, 0);
    } else {
      // fixed width
      cv.drawGlyph(X, Y, fontInfo->width, fontInfo->height, fontInfo->data, *text);
    }
  }
}

void showWiFiLoading(int n) {
  switch (n) {
    case 1:
      wifi = Bitmap(14, 15, &wifi_1_data[0], PixelFormat::Mask, RGB888(0, 0, 0));
      sprites[0].moveTo(52, 295);
      break;
    case 2:
      wifi = Bitmap(44, 35, &wifi_2_data[0], PixelFormat::Mask, RGB888(0, 0, 0));
      sprites[0].moveTo(37, 275);
      break;
    case 3:
      wifi = Bitmap(72, 55, &wifi_3_data[0], PixelFormat::Mask, RGB888(0, 0, 0));
      sprites[0].moveTo(23, 255);
      break;
    case 4:
      wifi = Bitmap(98, 75, &wifi_4_data[0], PixelFormat::Mask, RGB888(0, 0, 0));
      sprites[0].moveTo(10, 235);
      break;
    default:
      wifi = Bitmap(14, 15, &wifi_1_data[0], PixelFormat::Mask, RGB888(0, 0, 0));
      sprites[0].moveTo(52, 295);
      break;
  }
  sprites[0].visible = true;
  VGAController.refreshSprites();
}

void drawFile(int x, int y, const char *filename) {
  uint8_t w;
  uint8_t h;
  uint8_t p;
  fs::FS fs = SPIFFS;

  if (fs.exists(filename)) {
    File myFile = fs.open(filename);
    if (myFile) {
      w = myFile.read();        // read the dimension of the bitmap
      h = myFile.read();
      //Serial.printf("%dx%dpx \n", w, h);

      for (int8_t ypos = 0; ypos < h; ypos++, y++) {
        for (int8_t xpos = 0; xpos < w; xpos++) {
          p = myFile.read();
          uint8_t r = (p & 0x03) / 2 * 255;           // red  0 .. 255
          uint8_t g = ((p >> 2) & 0x03) / 2 * 255;    // green   0 .. 255
          uint8_t b = ((p >> 4) & 0x03) / 2 * 255;    // blue 0 .. 255
          uint8_t a = ((p >> 6) & 0x03) / 2 * 255;    // alpha  0 .. 255
          if (a == 0) continue;
          cv.setPixel(x + xpos, y, RGB888(r, g, b));
        }
      }
      myFile.close();           // all done, close the file
    } else {
      Serial.println("Failed to open file");
    }
  } else {
    Serial.println("File not found");
  }
}

void errorStatus(bool isError) {
  if (isError) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }
}
