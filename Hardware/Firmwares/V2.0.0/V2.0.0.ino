#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "fabgl.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define IR_PIN 4

WiFiMulti WiFiMulti;
fabgl::VGATextController DisplayController;
fabgl::Terminal Terminal;
WebSocketsClient webSocket;
DynamicJsonDocument doc(2048);

IRrecv irrecv(IR_PIN);
decode_results results;

unsigned long previousMillis = 0;
long interval = 1000;

void showNotification(String json) {
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  const char* time = doc["time"];
  previousMillis = millis();


  //  Terminal.write("\e[47;90m"); // background: white, foreground: black
  Terminal.write("\e[2J");     // clear screen
  Terminal.write("\e[1;73H");   // move cursor to 1,73
  Terminal.printf("%s", time); //Print time
  Terminal.write("\e[2;1H");

  if (doc["type"] == "NTF") {
    const char* name = doc["data"]["name"];
    const char* notification = doc["data"]["notification"];
    interval = doc["data"]["readingTime"];
    Terminal.printf("%s", name);
    Terminal.write("\e[4;1H");
    Terminal.printf("%s", notification);
    //    Terminal.write("\e[93m 5\e[4GHELLO WORLD\e[93m\e[45G45 dBm\e[55G45\e[60Gdsdfg saf\e[92m\r\n");
  } else if (doc["type"] == "CP") {
    interval = 10000;
    Terminal.write("\e[93m Period\e[4GSubject\e[93m\e[45GTeacher dBm\e[55GStartAt\e[60GEndAt saf\e[92m\r\n");
  } else if (doc["type"] == "WX") {
    interval = 10000;

    const char* timeStamp = doc["data"]["currentWeather"]["timestamp"];
    const char* location = doc["data"]["currentWeather"]["location"];
    const char* tempValue = doc["data"]["currentWeather"]["tempValue"];
    const char* phraseValue = doc["data"]["currentWeather"]["phraseValue"];
    Terminal.write("\e[4mCurrent Weather\e[0;92m\r\n\n");
    Terminal.printf("%s\r\n%s\r\n%s\r\n%s\r\n\n", timeStamp, location, tempValue, phraseValue);

    Terminal.write("\e[4mHourlyForcast\e[0;92m\r\n");
    Terminal.write("\e[90m Time\e[20GTemperature\e[45Chance To Rain\e[92m\r\n\n");
    for (int i = 0; i <= 4; i++) {
      /*
      const char* Htimestamp = doc["data"]["hourlyForcast"][i]["timestamp"];
      const char* HtempValue = doc["data"]["hourlyForcast"][i]["tempValue"];
      const char* HphraseValue = doc["data"]["hourlyForcast"][i]["precipValue"];
      Terminal.printf("\e[93m %s\e[20G%s\e[93m\e[45G%s\e[92m\r\n\n", Htimestamp, HtempValue, HphraseValue);
      */
      Terminal.write("\e[90m 12:NaN PM\e[20G28°\e[457%\e[92m\r\n\n");
    }
    Terminal.println(json);

  } else {

  }

}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      Terminal.println("\e[92mServer [WS] Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      Terminal.printf("\e[92mServer [WS] Connected to url: %s \n", payload);

      // send message to server when Connected
      webSocket.sendTXT("connected");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      //      Terminal.printf("[WSc] get text: %s\n", payload);
      showNotification((char*)payload);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void slowPrintf(const char * format, ...) {
  va_list ap;
  va_start(ap, format);
  int size = vsnprintf(nullptr, 0, format, ap) + 1;
  if (size > 0) {
    va_end(ap);
    va_start(ap, format);
    char buf[size + 1];
    vsnprintf(buf, size, format, ap);
    for (int i = 0; i < size; ++i) {
      Terminal.write(buf[i]);
      delay(25);
    }
  }
  va_end(ap);
}

void intro() {
  Terminal.write("\e[40;92m"); // background: black, foreground: green
  Terminal.write("\e[2J");     // clear screen
  Terminal.write("\e[1;1H");   // move cursor to 1,1
  slowPrintf("                    W E L C O M E   T O   S C N O T I F I E R\r\n");
  slowPrintf("============================ scnotifier.elabins.com ============================\r\n\n");
  slowPrintf("\e[4mCurrent Settings\e[0;92m\r\n");
  slowPrintf("Screen Size   : %d x %d\r\n", DisplayController.getScreenWidth(), DisplayController.getScreenHeight());
  slowPrintf("Terminal Size : %d x %d\r\n", Terminal.getColumns(), Terminal.getRows());
  slowPrintf("Free Memory   : %d bytes\r\n\n", heap_caps_get_free_size(MALLOC_CAP_32BIT));

  slowPrintf("\e[4mProject By\e[0;92m\r\n");
  slowPrintf("Mohammed Ashad\r\nAswin Rosh\r\nMohammed Rabeegu\r\nMohammed Afeed\r\nRabeebu Rahman\r\n\n");

  Terminal.print("WiFi connected to: ");
  Terminal.println(WiFi.SSID());
  Terminal.print("IP address: ");
  Terminal.println(WiFi.localIP());
}
void setup() {
  Serial.begin(115200); delay(500); Serial.write("\n\n\n"); // DEBUG ONLY

  WiFiMulti.addAP("e-labinnovations", "PASSWORD");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  DisplayController.begin(GPIO_NUM_12, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_26, GPIO_NUM_25);
  DisplayController.setResolution();

  Terminal.begin(&DisplayController);
  //Terminal.setLogStream(Serial);  // DEBUG ONLY

  //  Terminal.enableCursor(true);
  intro();

  // server address, port and URL
  webSocket.begin("scnotifier.herokuapp.com", 80, "/");
  // event handler
  webSocket.onEvent(webSocketEvent);
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  irrecv.enableIRIn();  // Start the receiver
}

void loop() {
  webSocket.loop();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    webSocket.sendTXT("next");
  }

  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
}
