//SPEAKER IS CONNECTED TO PIN 11

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
const int switchNoise = 4;

const int engine1 = 3;
const int engine2 = 5;
const int engine3 = 6;
const int engine4 = 9;

const int cannon = 8;

int engineFade1 = 255;
int engineFade2 = 255;
int engineFade3 = 255;
int engineFade4 = 255;

int screenMode = 0;

boolean fadeUp1 = false;
boolean fadeUp2 = false;
boolean fadeUp3 = false;
boolean fadeUp4 = false;

boolean Shot1 = false;
boolean Shot2 = false;
boolean Shot3 = false;
boolean Shot4 = false;

bool h12;
bool PM;

long fadeInterval = 50;
long previousMillisFade = 0;

long interval = 5000;
long previousMillis = 0;

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
  pinMode(engine3, OUTPUT);
  pinMode(engine4, OUTPUT);
  pinMode(cannon, OUTPUT);
}

void loop() {
  if (digitalRead(switchDST) == HIGH) {  //MAKE DST DO SOMETHING
    DST = true;
  } else {
    DST = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFade > fadeInterval) {
    previousMillisFade = currentMillis;
    engineLights();
  }

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    if (screenMode == 0) {
      screenMode = 1;
      display.setCursor(22, 41);
      display.println(F("That's no Moon,"));
      display.setCursor(1, 49);
      display.println(F("It's a Space Station!"));
    } else {
      screenMode = 0;
      display.setCursor(25, 41);
      display.println(F("May the Force"));
      display.setCursor(30, 49);
      display.println(F("Be With You!"));
    }
    display.display();
  }

  display.drawBitmap((display.width()  - COLON_WIDTH) / 2, 0, Colon, COLON_WIDTH, COLON_HEIGHT, 1);

  if (screenMode == 0) {
    display.drawBitmap(0, 0, Jedi, JEDI_WIDTH, JEDI_HEIGHT, 1);
    display.drawBitmap(96, 0, Jedi, JEDI_WIDTH, JEDI_HEIGHT, 1);
  } else if (screenMode == 1) {
    display.drawBitmap(0, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);
    display.drawBitmap(96, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);
  }

  displayTime();

  display.display();

  if ((Clock.getMinute() == (0 || 15 || 30 || 45)) || laserTime) {
    fireCannon(currentMillis);
  }

  if ((Clock.getMinute() != (0 || 15 || 30 || 45)) && (currentMillis - previousMillisEngine > engineDelay)) {
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

    if (engineFade1 >= 255) {
      fadeUp1 = false;
    }
  } else if (!fadeUp1) {
    engineFade1 = engineFade1 - fadeAmount1;

    if (engineFade1 >= 255) {
      fadeUp1 = true;
    }
  }

  if (fadeUp2) {
    engineFade2 = engineFade2 + fadeAmount2;

    if (engineFade2 >= 255) {
      fadeUp2 = false;
    }
  } else if (!fadeUp2) {
    engineFade2 = engineFade2 - fadeAmount2;

    if (engineFade2 >= 255) {
      fadeUp2 = true;
    }
  }

  if (fadeUp3) {
    engineFade3 = engineFade3 + fadeAmount3;

    if (engineFade3 >= 255) {
      fadeUp3 = false;
    }
  } else if (!fadeUp3) {
    engineFade3 = engineFade3 - fadeAmount3;

    if (engineFade3 >= 255) {
      fadeUp3 = true;
    }
  }

  if (fadeUp4) {
    engineFade4 = engineFade4 + fadeAmount4;

    if (engineFade4 >= 255) {
      fadeUp4 = false;
    }
  } else if (!fadeUp4) {
    engineFade4 = engineFade4 - fadeAmount4;

    if (engineFade4 >= 255) {
      fadeUp4 = true;
    }
  }

  engineFade1 = constrain(engineFade1, 0, 255);
  engineFade2 = constrain(engineFade2, 0, 255);
  engineFade3 = constrain(engineFade3, 0, 255);
  engineFade4 = constrain(engineFade4, 0, 255);

  analogWrite(engine1, engineFade1);
  analogWrite(engine2, engineFade2);
  analogWrite(engine3, engineFade3);
  analogWrite(engine4, engineFade4);
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

    if (digitalRead(switchNoise)) {
      startPlayback(laser, NUM_ELEMENTS_LASER);
    }
  }

  if (currentTime - previousMillisCannon > cannonDelay1) {
    digitalWrite(cannon, HIGH);
  }

  if (currentTime - previousMillisCannon > cannonDelay2) {
    digitalWrite(cannon, LOW);
  }

  if (currentTime - previousMillisCannon > cannonDelay3) {
    digitalWrite(cannon, HIGH);
  }

  if (currentTime - previousMillisCannon > cannonDelay4) {
    digitalWrite(cannon, LOW);
  }

  if (currentTime - previousMillisCannon > cannonDelay5) {
    digitalWrite(cannon, HIGH);
  }

  if (currentTime - previousMillisCannon > cannonDelay6) {
    digitalWrite(cannon, LOW);
  }

  if (currentTime - previousMillisCannon > cannonAudioLength) {
    firstLaser = true;
    laserTime = false;

    if (digitalRead(switchNoise)) {
      stopPlayback();
    }
  }
}
