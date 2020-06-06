#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PCM.h>

#include "Rebel.h"
#include "Jedi.h"
#include "Numbers.h"
#include "Laser.h"
#include "Engine.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4

void engineLights ();
void displayTime ();
void fireCannon (long currentTime);

const int switchDST = 2;
const int switchNoise = 7;

const int engine1 = 5;
const int engine2 = 6;

const int cannon = 4;

int engineFade1;
int engineFade2;

int engineMin = 10;
int engineMax = 255;

int screenMode = 0;

boolean fadeUp1 = false;
boolean fadeUp2 = false;

boolean cannonSafety1 = false;
boolean cannonSafety2 = false;
boolean cannonSafety3 = false;
boolean cannonSafety4 = false;
boolean cannonSafety5 = false;
boolean cannonSafety6 = false;

bool h12;
bool PM;

boolean firstTime = true;

long fadeInterval = 50;
long previousMillisFade = 0;

long interval = 5000;
long previousMillis = 0;

long cannonSafety = 60000;
long previousMillisCannonFire = 0;

long previousMillisCannon = 0;
long cannonDelay1 = 41;
long cannonDelay2 = 61;
long cannonDelay3 = 123;
long cannonDelay4 = 143;
long cannonDelay5 = 240;
long cannonDelay6 = 260;
long cannonAudioLength = 401;

boolean laserTime = false;
boolean firstLaser = true;

long previousMillisEngine = 0;
long engineDelay = 0;
long engineAudioLength = 1171;

boolean engineNoisePlay = false;

boolean DST = false;

DS3231 Clock;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(switchDST, INPUT);
  pinMode(switchNoise, INPUT);

  pinMode(engine1, OUTPUT);
  pinMode(engine2, OUTPUT);
  pinMode(cannon, OUTPUT);

  engineFade1 = random(engineMin, engineMax);
  engineFade2 = random(engineMin, engineMax);

  digitalWrite(cannon, LOW);
}

void loop() {
  if (digitalRead(switchDST) == HIGH) {
    DST = true;
  } else {
    DST = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFade > fadeInterval) {
    previousMillisFade = currentMillis;
    engineLights();
  }

  if ((currentMillis - previousMillis > interval) || firstTime) {
    firstTime = false;

    previousMillis = currentMillis;
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.drawBitmap((display.width()  - COLON_WIDTH) / 2, 0, Colon, COLON_WIDTH, COLON_HEIGHT, 1);

    if (screenMode == 0) {
      screenMode = 1;
      display.setCursor(22, 41);
      display.println(F("That's no Moon,"));
      display.setCursor(1, 49);
      display.println(F("It's a Space Station!"));
      display.drawBitmap(0, 0, Jedi, JEDI_WIDTH, JEDI_HEIGHT, 1);
      display.drawBitmap(96, 0, Jedi, JEDI_WIDTH, JEDI_HEIGHT, 1);
    } else {
      screenMode = 0;
      display.setCursor(25, 41);
      display.println(F("May the Force"));
      display.setCursor(30, 49);
      display.println(F("Be With You!"));
      display.drawBitmap(0, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);
      display.drawBitmap(96, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);
    }

    displayTime();
    display.display();
  }

  if (((Clock.getMinute() == 0) || (Clock.getMinute() == 15) || (Clock.getMinute() == 30) || (Clock.getMinute() == 45) || laserTime) && currentMillis - previousMillisCannonFire > cannonSafety) {

    fireCannon(currentMillis);
  }

  if (((Clock.getMinute() != 0) || (Clock.getMinute() != 15) || (Clock.getMinute() != 30) || (Clock.getMinute() != 45) || (Clock.getMinute() != 59) || (Clock.getMinute() != 14) || (Clock.getMinute() != 29) || (Clock.getMinute() != 44)) && (currentMillis - previousMillisEngine > engineDelay) && (digitalRead(switchNoise))) {
    previousMillisEngine = currentMillis;
    startPlayback(engine, NUM_ELEMENTS_ENGINE);

    engineDelay = random(60000, 3600000);
    engineNoisePlay = true;
  }

  if ((currentMillis - previousMillisEngine > engineAudioLength) && engineNoisePlay) {
    stopPlayback();

    engineNoisePlay = false;
  }
}

void engineLights () {
  fadeInterval = random(250);
  int fadeAmount1 = random(5);
  int fadeAmount2 = random(5);
  int fadeAmount3 = random(5);
  int fadeAmount4 = random(5);

  if (fadeUp1) {
    engineFade1 = engineFade1 + fadeAmount1;

    if (engineFade1 >= engineMax) {
      fadeUp1 = false;
    }
  } else if (!fadeUp1) {
    engineFade1 = engineFade1 - fadeAmount1;

    if (engineFade1 <= engineMin) {
      fadeUp1 = true;
    }
  }

  if (fadeUp2) {
    engineFade2 = engineFade2 + fadeAmount2;

    if (engineFade2 >= engineMax) {
      fadeUp2 = false;
    }
  } else if (!fadeUp2) {
    engineFade2 = engineFade2 - fadeAmount2;

    if (engineFade2 <= engineMin) {
      fadeUp2 = true;
    }
  }

  engineFade1 = constrain(engineFade1, engineMin, engineMax);
  engineFade2 = constrain(engineFade2, engineMin, engineMax);

  analogWrite(engine1, engineFade1);
  analogWrite(engine2, engineFade2);
}

void displayTime () {
  int hours = Clock.getHour(h12, PM);
  int minutes = Clock.getMinute();

  int h1;
  int h2;
  int m1;
  int m2;

  int pos1 = 33;
  int pos2 = 47;
  int pos3 = 67;
  int pos4 = 81;

  if (DST) {
    hours--;

    if (hours > 24) {
      hours = hours - 24;
    }
  }

  if (hours < 10) {
    h1 = 0;
    h2 = hours;
  } else {
    h1 = hours / 10;
    h2 = hours % 10;
  }

  if (minutes < 10) {
    m1 = 0;
    m2 = minutes;
  } else {
    m1 = minutes / 10;
    m2 = minutes % 10;
  }

  for (int i = 0; i < 4; i++) {
    int temp1;
    int temp2;

    if (i == 0) {
      temp1 = h1;
      temp2 = pos1;
    } else if (i == 1) {
      temp1 = h2;
      temp2 = pos2;
    } else if (i == 2) {
      temp1 = m1;
      temp2 = pos3;
    } else if (i == 3) {
      temp1 = m2;
      temp2 = pos4;
    }

    if (temp1 == 0) {
      display.drawBitmap(temp2, 0, Zero, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 1) {
      display.drawBitmap(temp2, 0, One, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 2) {
      display.drawBitmap(temp2, 0, Two, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 3) {
      display.drawBitmap(temp2, 0, Three, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 4) {
      display.drawBitmap(temp2, 0, Four, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 5) {
      display.drawBitmap(temp2, 0, Five, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 6) {
      display.drawBitmap(temp2, 0, Six, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 7) {
      display.drawBitmap(temp2, 0, Seven, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 8) {
      display.drawBitmap(temp2, 0, Eight, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    } else if (temp1 == 9) {
      display.drawBitmap(temp2, 0, Nine, NUMBER_WIDTH, NUMBER_HEIGHT, 1);
    }
  }
}

void fireCannon (long currentTime) {
  if (firstLaser) {
    firstLaser = false;
    laserTime = true;
    previousMillisCannon = currentTime;

    cannonSafety1 = true;
    cannonSafety2 = true;
    cannonSafety3 = true;
    cannonSafety4 = true;
    cannonSafety5 = true;
    cannonSafety6 = true;

    if (digitalRead(switchNoise)) {
      startPlayback(laser, NUM_ELEMENTS_LASER);
    }
  }

  if (currentTime - previousMillisCannon > cannonDelay1 && cannonSafety1) {
    digitalWrite(cannon, HIGH);
    cannonSafety1 = false;
  }

  if (currentTime - previousMillisCannon > cannonDelay2 && cannonSafety2) {
    digitalWrite(cannon, LOW);
    cannonSafety2 = false;
  }

  if (currentTime - previousMillisCannon > cannonDelay3 && cannonSafety3) {
    digitalWrite(cannon, HIGH);
    cannonSafety3 = false;
  }

  if (currentTime - previousMillisCannon > cannonDelay4 && cannonSafety4) {
    digitalWrite(cannon, LOW);
    cannonSafety4 = false;
  }

  if (currentTime - previousMillisCannon > cannonDelay5 && cannonSafety5) {
    digitalWrite(cannon, HIGH);
    cannonSafety5 = false;
  }

  if (currentTime - previousMillisCannon > cannonDelay6 && cannonSafety6) {
    digitalWrite(cannon, LOW);
    cannonSafety6 = false;
  }

  if (currentTime - previousMillisCannon > cannonAudioLength) {
    firstLaser = true;
    laserTime = false;

    previousMillisCannonFire = currentTime;

    stopPlayback();
  }
}
