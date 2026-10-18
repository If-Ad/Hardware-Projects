// C++ code
//
const int led1 = 13;
const int led2 = 12;
const int button = 8;
int button_state;
  
void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop()
{
  button_state = digitalRead(button);
  
  if (button_state == LOW) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
  } else {
    digitalWrite(led2, HIGH);
    digitalWrite(led1, LOW);
  }
  delay(500); // Wait for 1000 millisecond(s)
}