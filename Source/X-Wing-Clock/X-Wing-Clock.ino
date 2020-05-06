#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Rebel.h"

const int switchDST = 2;
const int switchNoise = 3;

const int engine1 = 4; //PB1;
const int engine2 = 5; //PB0;
const int engine3 = 6; //PA7;
const int engine4 = 7; //PA6;

const int cannon = 8; //PA5;

const int speaker = 9; //FIGURETHISOUT;

int engineFade1 = 255;
int engineFade2 = 255;
int engineFade3 = 255;
int engineFade4 = 255;

int screenMode = 0;

boolean fadeUp1 = false;
boolean fadeUp2 = false;
boolean fadeUp3 = false;
boolean fadeUp4 = false;

long fadeInterval = 50;
long previousMillisFade = 0;
long interval = 5000;
long previousMillis = 0;

boolean DST = false;

DS3231 Clock;

Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  display.clearDisplay();
  display.display();
  delay(100);
  display.clearDisplay();

  pinMode(switchDST, INPUT);
  pinMode(switchNoise, INPUT);

  pinMode(engine1, OUTPUT);
  pinMode(engine2, OUTPUT);
  pinMode(engine3, OUTPUT);
  pinMode(engine4, OUTPUT);
  pinMode(cannon, OUTPUT);
}

void loop() {

  //  if (digitalRead(switchDST) == HIGH) {
  //    if (!DST) {
  //      //      DS3231M.adjust(DateTime(now.year(), now.month(), now.day(), now.hour() + 1, now.minute(), now.second()));
  //    }
  //
  //    DST = true;
  //  } else {
  //    if (DST) {
  //      //     DS3231M.adjust(DateTime(now.year(), now.month(), now.day(), now.hour() - 1, now.minute(), now.second()));
  //    }
  //
  //    DST = false;
  //  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFade > fadeInterval) {
    previousMillisFade = currentMillis;
    engine();
  }

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (screenMode == 0) {
      screenMode = 1;
    } else {
      screenMode = 0;
    }
  }

  if (screenMode == 1) {  //Time Mode
    display.drawBitmap(0, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);
    display.drawBitmap(96, 0, Rebel, REBEL_WIDTH, REBEL_HEIGHT, 1);

  } else if (screenMode == 0) {  //Luke Mode

  }
}

void engine() {
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
