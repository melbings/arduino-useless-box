#include <Servo.h>

  /*
   * Finger nicht ganz hin, zögern, langsam zurückziehen, schnell umschalten und zurückziehen (man könnte auch mehrmals "foppen" bevor der Schalter tatsächlich umgelegt wird)
   * -> Ohne foppen am 23.01.21 gemacht
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

const int ARM_POS_BACK = 20;
const int ARM_POS_ALMOST = 97;
const int ARM_POS_ALMOSTBITBACK = 70;
const int ARM_POS_PUSH = 107;
const int ARM_POS_PUSHSTOPMOTOR = 103;
const int BOX_POS_CLOSED = 120;
const int BOX_POS_OPEN = 156;
const int BOX_POS_OPENLITTLEBIT = 140;
const int FLAG_POS_HIDDEN = 10;
const int FLAG_POS_LEFT = 55;
const int FLAG_POS_RIGHT = 110;


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
  
  box.write(BOX_POS_CLOSED);
  arm.write(ARM_POS_BACK);
  flag.write(FLAG_POS_HIDDEN);
  
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
      retractArm();
      closeBox();
      break;

    case 3:
      // Kurz vor dem Schalter zögern, dann drücken
      openBox();
      almostPushTheButton();
      delay(1600);
      pushTheButton(0, 0, 3);
      retractArm();
      closeBox();
      break;

    case 4:
      // Warten, dann normal
      delay(1600);
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 5:
      // Fanfare, öffnen, push
      fanfare();
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 6:
      // Normal
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;
      
    case 7:
      openBox();
      almostPushTheButton();
      delay(1000);
      for (int u = arm.read(); u > ARM_POS_ALMOSTBITBACK; u--) {
        arm.write(u);
        delay(10);
      }
      delay(200);
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 8:
      // Mit Nachdruck
      openBox();
      pushTheButton(400, 1500, 0);
      retractArm();
      closeBox();
      break;

    case 9:
      // Auf, zu, auf, push, zu
      openBox();
      delay(600);
      closeBox();
      delay(2000);
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 10:
      // Normal
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 11:
      // Einhämmern
      openBox();

      pushTheButton(300, 0, 0);
    
      for (int i = 0; i < 12; i++) {
        arm.write(85);
        delay(100);
    
        arm.write(110);
        delay(100);
      } 
    
      delay(1000);
      waitForButton(BUTTON_DOWN, false);

      retractArm();
      closeBox();
      break;

    case 12:
      // Normal
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      break;

    case 13:
      // Normal, dann nochmal nachschauen
      openBox();
      pushTheButton();
      retractArm();
      closeBox();
      openBoxALittleBit();
      delay(2500);
      closeBox();
      break;

    case 14:
      // Aufgabe
      openBox();
      delay(1000);
      surrender();
      pushTheButton();
      retractArm();
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
  for (int i = box.read(); i <= BOX_POS_OPEN; i++) {
    box.write(i);
    delay(10);
  }
}

void openBoxALittleBit() {
  for (int i = box.read(); i <= BOX_POS_OPENLITTLEBIT; i++) {
    box.write(i);
    delay(10);
  }  
}

void almostPushTheButton() {
  arm.write(ARM_POS_ALMOST);
  delay(350);
}

void pushTheButton() {
  pushTheButton(400, 0, 0);
}

void pushTheButton(int pressingDelay, int afterPressingDelay, int extraAngle) {
  arm.write(ARM_POS_PUSH + extraAngle);
  delay(pressingDelay);

  if (afterPressingDelay > 0) {
    arm.write(ARM_POS_PUSHSTOPMOTOR);
    delay(afterPressingDelay);
  }

  waitForButton(BUTTON_DOWN, false);
}

void retractArm() {
  retractArm(300);
}

void retractArm(int afterRetractingDelay) {
  arm.write(ARM_POS_BACK);
  delay(afterRetractingDelay);
}

void closeBox() {
  for (int i = box.read(); i >= BOX_POS_CLOSED; i--) {
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
      flag.write(FLAG_POS_LEFT);
      delay(500);    
    }

    flag.write(FLAG_POS_RIGHT);
    delay(500);
  }

  flag.write(FLAG_POS_HIDDEN);
  delay(1500);
}
