#include <Servo.h>

  /*
   * Finger nicht ganz hin, zögern, langsam zurückziehen, schnell umschalten und zurückziehen (man könnte auch mehrmals "foppen" bevor der Schalter tatsächlich umgelegt wird)
   * 
   * Falls möglich: Globaler Timer, falls x Sekunden kein User-Input kam, reagieren (z.B. aufgehen oder Finger raus)
   * 
   * Schalter umschalten, schnell schließen, dann aber gleich erneut öffnen und Finger bis knapp vor Schalter fahren und hier warten
   * 
   * gaaaaaaanz langsam hinfahren, dann recht flott umlegen und zurückziehen
   * 
   * Schalter umlegen, schließen, kurz warten und dann erneut schnell bis ganz raus fahren und den Schalter "umlegen"
   * 
   */

const int BUTTON_PIN = A4;
const int BUTTON_UP = HIGH;
const int BUTTON_DOWN = LOW;

const int LED_BLUE_PIN = 3;
const int LED_RED_PIN = 4;

const int PHOTO_PIN = A5;

const int SERVO_BOX_PIN = 9;
const int SERVO_ARM_PIN = 10;
const int SERVO_FLAG_PIN = 11;

const int SOUND_PIN = 8;
const int SOUND_C4 = 262;
const int SOUND_E4 = 330;
const int SOUND_G4 = 392;
const int SOUND_C5 = 523;

Servo arm;
Servo box;
Servo flag;

int darkPhotoValue;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);

  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);
  
  arm.attach(SERVO_ARM_PIN);
  box.attach(SERVO_BOX_PIN);
  flag.attach(SERVO_FLAG_PIN);
  
  box.write(120);
  arm.write(20);
  flag.write(10);

  delay(3000);

  waitForButton(BUTTON_DOWN, false);
  determineDarkPhotoValue();
}

void determineDarkPhotoValue() {
  tone(SOUND_PIN, SOUND_C4); delay(200);
  noTone(SOUND_PIN);

  int photoValueSum = 0;
  for (int i = 0; i < 50; i++) {
    int photoValue = analogRead(PHOTO_PIN);
    delay(10);
    
    photoValueSum += photoValue;
  }

  int averagePhotoValue = photoValueSum / 50;
  darkPhotoValue = averagePhotoValue + 5;
  
  tone(SOUND_PIN, SOUND_C4); delay(200);
  noTone(SOUND_PIN);
}
  
int stage = 0;

void loop() {
  waitForButton(BUTTON_UP, true);
  stage++;

  switch (stage) {
    case 1:
    case 2:
      // Normal
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 3:
      // Warten, dann normal
      delay(1600);
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 4:
      // Öffnen, Fanfare, Push
      fanfare();
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 5:
      // Normal
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 6:
      // Mit Nachdruck
      openBox();
      pushTheButton(1500);
      closeBox();
      break;

    case 7:
      // Auf, zu, auf, push, zu
      openBox();
      delay(600);
      closeBox();
      delay(2000);
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 8:
      // Normal
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 9:
      // Einhämmern
      openBox();
      pushTheButtonRepeatedly();
      closeBox();
      break;

    case 10:
      // Normal
      openBox();
      pushTheButton();
      closeBox();
      break;

    case 11:
      // Normal
      openBox();
      pushTheButton();
      closeBox();
      openBoxALittleBit();
      delay(2500);
      closeBox();
      break;

    case 12:
      // Aufgabe
      openBox();
      delay(1000);
      surrender();
      pushTheButton();
      closeBox();
      stage = 0;
      break;
  }
}

void waitForButton(int state, bool warnWhenOpen) {
  while (true) {
    int photoValue = analogRead(PHOTO_PIN);
    delay(10);
    
    if (warnWhenOpen && photoValue >= darkPhotoValue) {
      digitalWrite(LED_BLUE_PIN, HIGH);
      digitalWrite(LED_RED_PIN, LOW);
      tone(SOUND_PIN, SOUND_C4);
      delay(100);
      
      digitalWrite(LED_BLUE_PIN, LOW);
      digitalWrite(LED_RED_PIN, HIGH);
      tone(SOUND_PIN, SOUND_E4);
      delay(100);

      digitalWrite(LED_BLUE_PIN, LOW);
      digitalWrite(LED_RED_PIN, LOW);
      noTone(SOUND_PIN);

      continue;
    }
     
    if (digitalRead(BUTTON_PIN) == state) {
      return;
    }
  }
}

void openBox() {
  for (int i = 120; i <= 156; i++) {
    box.write(i);
    delay(10);
  }
}

void openBoxALittleBit() {
  for (int i = 120; i <= 140; i++) {
    box.write(i);
    delay(10);
  }  
}

void pushTheButton() {
  pushTheButton(0);
}

void pushTheButton(int extraTimeOnButton) {
  arm.write(110);
  delay(400 + extraTimeOnButton);

  arm.write(20);
  delay(300);

  waitForButton(BUTTON_DOWN, false);
}

void pushTheButtonRepeatedly() {
  arm.write(110);
  delay(300);

  for (int i = 0; i < 12; i++) {
    arm.write(85);
    delay(100);

    arm.write(110);
    delay(100);
  } 

  delay(1000);

  arm.write(20);
  delay(300);

  waitForButton(BUTTON_DOWN, false);
}

void closeBox() {
    for (int i = 156; i >= 120; i--) {
    box.write(i);
    delay(10);
  }
}

void fanfare() {
  tone(SOUND_PIN, SOUND_C4); delay(400);
  noTone(SOUND_PIN);
  delay(200);

  tone(SOUND_PIN, SOUND_E4); delay(400);
  noTone(SOUND_PIN);
  delay(200);
 
  tone(SOUND_PIN, SOUND_G4); delay(400);
  noTone(SOUND_PIN);
  delay(200);
 
  tone(SOUND_PIN, SOUND_C5); delay(1200);  
  noTone(SOUND_PIN);
}

void flashRedAndBlue() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BLUE_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);
    delay(100);
    digitalWrite(LED_BLUE_PIN, LOW);
    digitalWrite(LED_RED_PIN, HIGH);
    delay(100);
  }
  
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
}

void surrender() {
  // Idee für Sound dabei: https://www.youtube.com/watch?v=Bfe4TxvUOiw

  for (int i = 0; i < 5; i++) {
    if (i > 0) {
      flag.write(55);
      delay(500);    
    }

    flag.write(110);
    delay(500);
  }

  flag.write(10);
  delay(1500);
}
