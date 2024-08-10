#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin positions for wireing preference.
const int relayPin = 6;
const int beeperPin = 7;
const int buttonPin = 5;

// Centrifuge specific duration of stages in milliseconds.
const long timeSpinning = 900000;
const long timeSpinUp = 20000;
const long timeSpinDown = 20000;

// Store device states and values for stage duration calculations.
bool buttonState = 0;
bool buttonLastState = 0;
int stage = 0;
int stageLast = 0;
long timeDurationSequence = 0;
long timeStartSequence = 0;

// LCD display hardware specific parameters.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Setup digital pins.
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Initialize LCD display.
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Store button state.
  buttonState = digitalRead(buttonPin);
  // Check if button state is on and previous button state is off.
  if (buttonState == 1 && buttonLastState == 0) {
    // Change state to spin-up, if the device is in ready state.
    if (stage == 0) {
      stage = 1;
    // Change state to abort, if the device is in any other state.
    } else {
      stage = 4;
    }
  }
  // Store the stored button state as last button state for next iteration.
  buttonLastState = buttonState;
  // Routine is selected depending on device state.
  switch (stage) {
    // Ready stage.
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Ready        ");
      lcd.setCursor(0, 1);
      lcd.print(String(timeSpinning/1000) + "s set");
      break;
    // Spin-up stage.
    case 1:
      // Reset sequence start time, if state has changed.
      if (stage != stageLast) {
        timeStartSequence = millis();
      }
      // Calculate sequence duration time.
      timeDurationSequence = millis() - timeStartSequence;
      // Execute state actions, if state duration has not elapsed.
      if (timeDurationSequence <= timeSpinUp) {
        // Indicate no state change.
        stageLast = stage;
        // Power on centrifuge.
        digitalWrite(relayPin, 1);
        // Display state and remaining time.
        lcd.setCursor(0, 0);
        lcd.print("Spin-up      ");
        lcd.setCursor(0, 1);
        lcd.print(String((timeSpinUp - timeDurationSequence) / 1000) + "s       ");
      } else {
        // Continue to spinning stage.
        stage = 2;
      }
      break;
    // Spinning stage.
    case 2:
      // Reset sequence start time, if state has changed.
      if (stage != stageLast) {
        timeStartSequence = millis();
      }
      // Calculate sequence duration time.
      timeDurationSequence = millis() - timeStartSequence;
      // Execute state actions, if state duration has not elapsed.
      if (timeDurationSequence <= timeSpinning) {
        // Indicate no state change.
        stageLast = stage;
        // Power on centrifuge.
        digitalWrite(relayPin, 1);
        // Display state and remaining time.
        lcd.setCursor(0, 0);
        lcd.print("Spinning      ");
        lcd.setCursor(0, 1);
        lcd.print(String((timeSpinning - timeDurationSequence) / 1000) + "s       ");
      } else {
        // Continue to spin-down stage.
        stage = 3;
      }
      break;
    // Spin down stage.
    case 3:
      // Reset sequence start time, if state has changed.
      if (stage != stageLast) {
        timeStartSequence = millis();
      }
      // Calculate sequence duration time.
      timeDurationSequence = millis() - timeStartSequence;
      // Execute state actions, if state duration has not elapsed.
      if (timeDurationSequence <= timeSpinDown) {
        // Indicate no state change.
        stageLast = stage;
        // Power off centrifuge.
        digitalWrite(relayPin, 0);
        // Display state and remaining time.
        lcd.setCursor(0, 0);
        lcd.print("Spin-Down      ");
        lcd.setCursor(0, 1);
        lcd.print(String((timeSpinDown - timeDurationSequence) / 1000) + "s       ");
      } else {
        // Continue to ready stage.
        stage = 0;
        // Beep to indicate end of spin-down.
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
        delay(200);
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
        delay(200);
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
      }
      break;
    // Abort stage.
    case 4:
      // Reset sequence start time, if state has changed.
      if (stage != stageLast) {
        timeStartSequence = millis();
        // Beep to indicate beginning of abort sequence.
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
      }
      // Calculate sequence duration time.
      timeDurationSequence = millis() - timeStartSequence;
      // Execute state actions, if state duration has not elapsed.
      if (timeDurationSequence <= timeSpinDown) {
        // Indicate no state change.
        stageLast = stage;
        // Power off centrifuge.
        digitalWrite(relayPin, 0);
        // Display state and remaining time.
        lcd.setCursor(0, 0);
        lcd.print("Abort      ");
        lcd.setCursor(0, 1);
        lcd.print(String((timeSpinDown - timeDurationSequence) / 1000) + "s       ");
      } else {
        // Continue to ready stage.
        stage = 0;
        // Beep to indicate end of spin-down.
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
        delay(200);
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
        delay(200);
        tone(beeperPin, 800);
        delay(200);
        noTone(beeperPin);
      }
      break;
  }
} 