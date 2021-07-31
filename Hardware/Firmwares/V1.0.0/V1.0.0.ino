#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/CodePage437_8x14.h>
#include <Ressources/CodePage437_8x16.h>
#include <Ressources/CodePage437_8x19.h>
#include <Ressources/CodePage437_9x16.h>
#include <Ressources/Font6x8.h>
#include <ArduinoJson.h>

//pin configuration
#define redPin  12
#define  greenPin  14
#define  bluePin  27
#define  hsyncPin  26
#define  vsyncPin  25

//VGA Device
VGA3Bit vga;
DynamicJsonDocument doc(2048);
String json = "[{\"name\":\"gptcthirurangadi.in\",\"notification\":\"Polytechnic Admission 2020-21 - Online spot registration on December 1st \\nand 2nd... Second Spot Admission will be held on Friday 04/12/2020 at this \\ninstitute functioning at Chelari. (01-12-2020) ::: ONLINE FEES PAYMENT -To \\npay the Admission fee, you need to come with an ATM card and cooperate with \\nthe digital payment. (30-10-2020)\",\"show\":true,\"readingTime\":12000},{\"name\":\"tekerala.org\",\"notification\":\"Error, Report to admin\",\"show\":true,\"readingTime\":5000},{\"name\":\"sitttrkerala.ac.in\",\"notification\":\"GIFD Admission 2020-21 - Application Form\\nGCI Admission 2020-21 - Application Form\\nGIFD Admission 2020-21 - Application Submission has been extended upto \\n28/11/2020.\\nGCI Admission 2020-21 - Application Submission has been extended upto \\n06/11/2020.\\nNational Web Conference - Best Paper Presenters\\nGIFD Admission - Revised Dates\\nGCI Admission - Revised Dates\\nKGTE Printing Course 2020-21 - Application Form\\nKGTE Printing Course 2020-21 - Prospectus\\nYouTube videos of Online classes for Diploma Students are available in \\nSITTTR Website\",\"show\":true,\"readingTime\":15000},{\"name\":\"sbte.kerala.gov.in\",\"notification\":\" No information is available/active for section now \",\"show\":true,\"readingTime\":5000},{\"name\":\"Class\",\"notification\":\"test\",\"show\":true,\"readingTime\":5000},{\"name\":\"HOD\",\"notification\":\"No notification available\",\"show\":true,\"readingTime\":5000},{\"name\":\"College\",\"notification\":\"Test notification from HOD\",\"show\":true,\"readingTime\":5000}]";


void setup() {
  Serial.begin(115200);
  //initializing vga at the specified pins
  vga.init(vga.MODE640x400, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
}

void loop() {
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    vga.print("deserializeJson() failed");
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  for (int i = 0; i <= 6; i++) {
    const char* name = doc[i]["name"];
    const char* notification = doc[i]["notification"];
    long readingTime = doc[i]["readingTime"];
    
    vga.clear();
    vga.setCursor(0,0);
    //selecting the font
    vga.setFont(CodePage437_9x16);
    vga.setTextColor(vga.RGB(255, 0, 255), vga.RGB(255, 0, 0));
    vga.println(name);
    vga.setFont(CodePage437_8x19);
    vga.setTextColor(vga.RGB(255, 255, 255));
    vga.println();
    vga.println(notification);
    delay(readingTime);
  }
}
