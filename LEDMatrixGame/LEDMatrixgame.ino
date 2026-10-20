#include <LedControl.h>

// Pin connections:
// Pin 12 -> DIN (DataIn)
// Pin 11 -> CLK
// Pin 10 -> LOAD (CS)
// Pin 6  -> Buzzer (active HIGH)

LedControl lc = LedControl(12, 11, 10, 1);  // DIN, CLK, CS, numDevices

// Maximum worm length
const int MAX_SEGMENTS = 10;

// Worm segments (head is index 0, tail is last index)
int segX[10];
int segY[10];
int segCount = 2;  // starts with 2 segments (head + tail)
int collisionCount = 0;  // counts how many times head eats target

// Target position
int targetX;
int targetY;

// Button pins
const int buttonUp = 13;
const int buttonDown = 7;
const int buttonRight = 8;
const int buttonLeft = 2;

// Buzzer pin
const int buzzerPin = 6;

int buttonUp_state;
int buttonDown_state;
int buttonRight_state;
int buttonLeft_state;

void setup() {
  // Button setup (INPUT_PULLUP: pressed = LOW)
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  
  // Buzzer setup
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);  // Start with buzzer OFF

  // Initialize MAX7219
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  
  // Seed random generator
  randomSeed(analogRead(A0));  // A0 should be unconnected/floating
  
  // Initialize worm (head and tail start at same position)
  segX[0] = 2;
  segY[0] = 3;
  segX[1] = 2;
  segY[1] = 3;
  
  // Place first target
  placeNewTarget();
}

void loop() {
  // Save current head position before moving
  int prevHeadX = segX[0];
  int prevHeadY = segY[0];
  bool moved = false;

  // Read buttons (INPUT_PULLUP: pressed = LOW)
  buttonUp_state = digitalRead(buttonUp);
  buttonDown_state = digitalRead(buttonDown);
  buttonRight_state = digitalRead(buttonRight);
  buttonLeft_state = digitalRead(buttonLeft);
 
  // Move head
  if (buttonUp_state == LOW)    { segY[0] = segY[0] - 1; moved = true; }
  if (buttonDown_state == LOW)  { segY[0] = segY[0] + 1; moved = true; }
  if (buttonRight_state == LOW) { segX[0] = segX[0] - 1; moved = true; }
  if (buttonLeft_state == LOW)  { segX[0] = segX[0] + 1; moved = true; }

  // Wrap around screen edges
  if (segX[0] > 7) segX[0] = 0;
  if (segX[0] < 0) segX[0] = 7;
  if (segY[0] > 7) segY[0] = 0;
  if (segY[0] < 0) segY[0] = 7;

  // Move tail segments only if head moved
  if (moved) {
    // Save current tail position (before shifting)
    int oldTailX = segX[segCount-1];
    int oldTailY = segY[segCount-1];
    
    // Shift all segments forward (tail follows head)
    for (int i = segCount-1; i > 0; i--) {
      segX[i] = segX[i-1];
      segY[i] = segY[i-1];
    }
    
    // Check for collision with target
    if (segX[0] == targetX && segY[0] == targetY) {
      collisionCount++;
      
      // BEEP THE BUZZER WHEN TARGET IS EATEN
      digitalWrite(buzzerPin, HIGH);  // Turn buzzer ON
      delay(50);                      // Beep for 50ms
      digitalWrite(buzzerPin, LOW);   // Turn buzzer OFF
      
      // Grow worm: add new segment at old tail position
      segX[segCount] = oldTailX;
      segY[segCount] = oldTailY;
      segCount++;  // increase segment count
      
      // Prevent excessive growth
      if (segCount > MAX_SEGMENTS) {
        // Reset worm when it gets too long
        segCount = 2;
        segX[0] = 2; segY[0] = 3;
        segX[1] = 2; segY[1] = 3;
        collisionCount = 0;
        placeNewTarget();
      }
      
      // Place new target after eating
      placeNewTarget();
    }
  }

  // Draw the worm (all segments)
  lc.clearDisplay(0);
  for (int i = 0; i < segCount; i++) {
    lc.setLed(0, segX[i], segY[i], true);
  }
  
  // Draw the target
  lc.setLed(0, targetX, targetY, true);
  
  delay(200);
}

// Place target at random position (avoiding worm body)
void placeNewTarget() {
  boolean validPosition;
  do {
    validPosition = true;
    targetX = random(0, 8);  // 0-7
    targetY = random(0, 8);  // 0-7
    
    // Check if target would spawn on worm
    for (int i = 0; i < segCount; i++) {
      if (targetX == segX[i] && targetY == segY[i]) {
        validPosition = false;
        break;
      }
    }
  } while (!validPosition);
}