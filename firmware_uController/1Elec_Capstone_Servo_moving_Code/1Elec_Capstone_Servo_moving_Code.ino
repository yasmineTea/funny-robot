#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>   // For DS1307
#include <Wire.h>
#include <time.h>
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);   // initialize board1 as the correct board

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS1307 rtc;
// OLED refresh timing
unsigned long lastDisplayUpdateMs = 0;
#define SERVOMIN 125  // Minimum pulse length (0 degrees)
#define SERVOMAX 625  // Maximum pulse length (180 degrees)
int angleToPulse(int ang);
int readAngle(int servoNumber);
int channel = 0;

int L_1 = 0;
int L_2 = 1;
int L_3 = 2;
int R_1 = 3;
int R_2 = 4;
int R_3 = 5;


void setup() {
  // serial init
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");
  Serial.println(F("PCA9685 + Servos + SSD1306 + DS1307 RTC"));
  pinMode(13, OUTPUT);
  
  // set board1 as the master control
  board1.begin();
  board1.setPWMFreq(60); // Servos operate at ~60 Hz
   // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed (check wiring/address)."));
    while (true) { delay(10); }
  }

  // rtc
  rtc.begin();
  // uncomment below to set the date and time yyyy, mon, day, hour, min, sec
  // rtc.adjust(DateTime(2026, 4, 7, 5, 50, 0));

  display.clearDisplay();
  display.setTextSize(1.2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("NINODUINO BOOTING UP"));
  //display.println(F("RTC: DS1307"));
  display.display();
  delay(400);
  // DS1307 init
  /*if (!rtc.begin()) {
    Serial.println(F("Couldn't find DS1307 RTC (check wiring)."));
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("DS1307 not found"));
    display.display();
    while (true) { delay(10); }
  }*/

  // If RTC isn't running (often after battery/power loss), set it to compile time
  /*if (!rtc.isrunning()) {
    Serial.println(F("DS1307 is NOT running. Setting time to compile time..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }*/
  //Set all servos to 0 degrees
  for(int i=0; i<16; i++) {
  board1.setPWM(i, 0, angleToPulse(90));
  }
  delay(1000);
}

void loop() {
  updateTimeDisplayIfDue();   // update clock

  //Serial.println(now.second());

  if (Serial.available() > 0) {
    L_1 = Serial.parseInt(); 
    L_2 = Serial.parseInt();
    L_3 = Serial.parseInt();
    R_1 = Serial.parseInt();
    R_2 = Serial.parseInt();
    R_3 = Serial.parseInt();
    
    // Clear the remaining newline from buffer
    while(Serial.available() > 0) Serial.read(); 

    L_1 = invertAngles(L_1);
    L_2 = map(L_2, 0, 90, 90, 0);

    // R_1 = invertAngles(R_1);
    R_2 = invertAngles(R_2);
    
    // board1.setPWM(4, 0, angleToPulse(L_1));    // shoulder forward and back
    board1.setPWM(3, 0, angleToPulse(L_2));       // elbow
    board1.setPWM(5, 0, angleToPulse(L_3));       // shoulder up and down
    

    // board1.setPWM(1, 0, angleToPulse(R_1));    // shoulder forward and back
    board1.setPWM(0, 0, angleToPulse(R_2));       // elbow
    board1.setPWM(2, 0, angleToPulse(R_3));       // shoulder up and down


  }
  

  

  // left hand connection to D13 onboard LED
  /*
  if (Serial.available() > 0)
  {
    int option = Serial.read();
    if (option == 'a')
    {
      digitalWrite(13, HIGH);    
    }
    if (option == 'b')
    {
      digitalWrite(13, LOW);
    }
  }
  */




  /*
  for (int i = 45; i <= 135; i = i+2 ) {
      board1.setPWM(L_1, 0, angleToPulse(i));
      Serial.println(i);
      delay(100);
  }
  delay(500);
  board1.setPWM(L_1, 0, 90);
  delay(500);

  for (int i = 45; i <= 135; i = i+2 ) {
        board1.setPWM(L_2, 0, angleToPulse(i));
        Serial.println(i);
        delay(100);
  }
  delay(500);
  board1.setPWM(L_2, 0, 90);
  delay(500);
  */


  /*
  updateTimeDisplayIfDue();
  // Sweep all servos from 0 to 180 degrees
  for(int angle = 0; angle < 91; angle += 15) {
    for(int i=0; i<8; i++) {
      updateTimeDisplayIfDue();
      board1.setPWM(i, 0, angleToPulse(angle));
    }
  }
  updateTimeDisplayIfDue();
  for(int angle = 90; angle > 0; angle -= 15) {
    for(int i=0; i<8; i++) {
      updateTimeDisplayIfDue();
      board1.setPWM(i, 0, angleToPulse(angle));
    }
  }
  */

  /*for (int i=0; i<3; i++)
  {
    int angle = readAngle(i);
    board1.setPWM(i, 0, angleToPulse(angle));
    delay(400);
  }
  Serial.println(F("Done moving servos, starting again\n"));
  */
}

/*
int readAngle(int servoNumber)
{
  while (true)
  {
    updateTimeDisplayIfDue();
    Serial.println(F("Enter an angle for servo "));
    Serial.print(servoNumber);
    Serial.println(F("(0-180) then press Enter: "));
    while (Serial.available() == 0)
    {

    }

    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() == 0)
    {
      Serial.println(F("No input, please try again"));
      continue;
    }

    bool numeric = true;
    for (unsigned int i=0; i<line.length(); i++)
    {
      if (!isDigit(line[i]))
      {
        numeric = false;
        break;
      }
    }
    if (!numeric)
    {
      Serial.println(F("Not a number, please try again"));
      continue;
    }

    int ang = line.toInt();
    if (ang < 0 || ang > 180)
    {
      Serial.println(F("Out of angle range, enter 0-180"));
      continue;
    }
    Serial.print(F("Servo "));
    Serial.print(servoNumber);
    Serial.print(F("New servos angle: "));
    Serial.println(ang);
    Serial.println("\n");
    return ang;
  }
}
*/
int invertAngles(int ang) {
  int inverted = map(ang, 0, 180, 180, 0);
  return inverted;
}

int angleToPulse(int ang) {
  int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX);
  // Serial.print("Angle: "); Serial.print(ang);
  // Serial.print("Pulse: "); Serial.println(pulse);

  // if not valid angle, return original value
  if (ang > 180) {
    return;
  }
  else if (ang < 0) {
    return;
  }
  else {
    return pulse;
  }
  return;
}
// ---------------- OLED time display (DS1307) ----------------
void updateTimeDisplayIfDue() {
  unsigned long nowMs = millis();
  DateTime now = rtc.now();

  if (nowMs - lastDisplayUpdateMs < 1000) return; // ~1 Hz
  lastDisplayUpdateMs = nowMs;
  // Uptime HH:MM:SS
  /*
  unsigned long totalSeconds = nowMs / 1000UL;
  unsigned int hh = (totalSeconds / 3600UL) % 1000; // big uptime safe
  unsigned int mm = (totalSeconds / 60UL) % 60;
  unsigned int ss = totalSeconds % 60;
  */

  display.clearDisplay();
  display.setCursor(8, 0);
  display.setTextSize(1);
  display.println(F("NINODUINO  RTC TIME"));

  display.setTextSize(2);
  display.setCursor(16, 18);
  char upStr[12];
  snprintf(upStr, sizeof(upStr), "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
  display.println(upStr);

  display.setCursor(96, 45);
  display.setTextSize(0.5);
  display.print(now.year());

  display.setCursor(10, 45);
  display.setTextSize(0.5);
  display.println(now.day());

  display.setCursor(26, 45);
  display.setTextSize(0.5);
  display.println(now.month());  

  display.setTextSize(1);
  display.setCursor(42, 40);
  display.println(" /|_|\\ ");
  display.setCursor(36, 50);

  if (now.second() == 0) {          // cute blink animation
    display.println(F(" |- o -| "));
  }
  else if (now.second() % 5 == 0) {
    display.println(F(" |- w -| "));
  }
  else {
    display.println(F(" |o w o| "));
  }
  
  

  display.drawRect(8, 10, 112, 28, WHITE);

/*
  DateTime now = rtc.now();

  char timeStr[9];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
           now.hour(), now.minute(), now.second());

  char dateStr[11];
  snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d",
           now.year(), now.month(), now.day());

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("TIME (DS1307)"));

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.println(timeStr);

  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(dateStr);
*/
  display.display();
}