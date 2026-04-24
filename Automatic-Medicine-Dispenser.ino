  #include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <DHT.h>

// Servos
Servo morning;
Servo afternoon;
Servo night;

int closedAngle = 90;
int openAngle = 0;

// IR Sensor pins
int irMorning   = 2;
int irAfternoon = 3;
int irNight     = 8;

// Buzzer
int buzzerPin = 11;

// DHT11
#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic
#define TRIGPIN 13
#define ECHOPIN A0
#define PERSON_DISTANCE 100  // cm

// Flags
bool morningDone     = false;
bool afternoonDone   = false;
bool nightDone       = false;
bool morningTaken    = false;
bool afternoonTaken  = false;
bool nightTaken      = false;

// *** TEST VALUES ***
int testHour   = 7;
int testMinute = 59;
int testSecond = 50;

unsigned long startMillis = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

ThreeWire myWire(4, 5, 6);
RtcDS1302<ThreeWire> Rtc(myWire);

// ---- Beeps ----
void singleBeep() {
  digitalWrite(buzzerPin, HIGH); delay(200); digitalWrite(buzzerPin, LOW);
}

void tripleBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH); delay(200); digitalWrite(buzzerPin, LOW);
    delay(200);
  }
}

void longBeep() {
  digitalWrite(buzzerPin, HIGH); delay(3000); digitalWrite(buzzerPin, LOW);
}

// ---- Ultrasonic ----
float getDistance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  long duration = pulseIn(ECHOPIN, HIGH, 30000);
  return duration * 0.034 / 2;
}

bool isPersonNearby() {
  float dist = getDistance();
  return (dist > 0 && dist < PERSON_DISTANCE);
}

// ---- Storage Conditions ----
void checkStorageConditions() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Hum: ");  Serial.println(hum);

  if (isnan(temp) || isnan(hum)) return;

  if (temp > 40.0) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP TOO HIGH!");
    lcd.setCursor(0, 1);
    lcd.print(temp); lcd.print(" C");
    longBeep();
    delay(3000);
  }

  if (hum > 60.0) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HUMIDITY HIGH!");
    lcd.setCursor(0, 1);
    lcd.print(hum); lcd.print(" %");
    longBeep();
    delay(3000);
  }
}

// ---- Person Display ----
void showPersonDisplay(int h, int m, int s) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(h < 10 ? "0" : ""); lcd.print(h);
  lcd.print(":");
  lcd.print(m < 10 ? "0" : ""); lcd.print(m);
  lcd.print(":");
  lcd.print(s < 10 ? "0" : ""); lcd.print(s);
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}

// ---- Wait for Medicine ----
void waitForMedicineTaken(int irPin, Servo &servo, String label, bool &takenFlag) {
  while (digitalRead(irPin) == LOW) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Take your");
    lcd.setCursor(0, 1);
    lcd.print(label + " medicine!");
    tripleBeep();
    delay(5000);
  }

  takenFlag = true;
  singleBeep();
  servo.write(closedAngle);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Medicine Taken!");
  lcd.setCursor(0, 1);
  lcd.print("Box Closed.");
  delay(2000);
}

// ---- Open Compartment ----
void openCompartment(Servo &servo, int irPin, String label, bool &takenFlag) {
  singleBeep();
  servo.write(openAngle);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label + " Time!");
  lcd.setCursor(0, 1);
  lcd.print("Box Opening...");
  delay(1500);

  waitForMedicineTaken(irPin, servo, label, takenFlag);
}

void setup() {
  Serial.begin(9600);
  morning.attach(7);
  afternoon.attach(9);
  night.attach(10);

  morning.write(closedAngle);
  afternoon.write(closedAngle);
  night.write(closedAngle);

  pinMode(irMorning,   INPUT);
  pinMode(irAfternoon, INPUT);
  pinMode(irNight,     INPUT);
  pinMode(buzzerPin,   OUTPUT);
  pinMode(TRIGPIN,     OUTPUT);
  pinMode(ECHOPIN,     INPUT);

  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Medicine Box");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");
  delay(2000);
  lcd.noBacklight();  // turn off after startup

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);

  startMillis = millis();
}

void loop() {
  unsigned long elapsed = (millis() - startMillis) / 1000;

  long totalSeconds = (long)testHour * 3600 + (long)testMinute * 60 + testSecond + elapsed;

  int h = (totalSeconds / 3600) % 24;
  int m = (totalSeconds % 3600) / 60;
  int s = totalSeconds % 60;

  // reset flags at midnight
  if (h == 0 && m == 0 && s == 0) {
    morningDone    = false;
    afternoonDone  = false;
    nightDone      = false;
    morningTaken   = false;
    afternoonTaken = false;
    nightTaken     = false;
  }

  // always check storage conditions
  checkStorageConditions();
  
  // Morning - 8:00 AM
  if (h == 8 && m == 0 && !morningDone) {
    morningDone = true;
    lcd.backlight();
    openCompartment(morning, irMorning, "Morning", morningTaken);
  }

  // Afternoon - 2:00 PM
  if (h == 14 && m == 0 && !afternoonDone) {
    afternoonDone = true;
    lcd.backlight();
    openCompartment(afternoon, irAfternoon, "Afternoon", afternoonTaken);
  }

  // Night - 8:00 PM
  if (h == 20 && m == 0 && !nightDone) {
    nightDone = true;
    lcd.backlight();
    openCompartment(night, irNight, "Night", nightTaken);
  }

  // ultrasonic controls LCD
  if (isPersonNearby()) {
    lcd.backlight();
    showPersonDisplay(h, m, s);
  } else {
    lcd.noBacklight();
    lcd.clear();
  }

  delay(500);
}