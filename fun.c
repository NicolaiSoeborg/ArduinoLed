const int soundPin = 1; // Alias "TX"

void setup() {
  int btnPin = 3; // Listen for button click
  pinMode(btnPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(btnPin), inputHandler, FALLING);

  pinMode(soundPin, INPUT);

  // 9..12 is used for light, 13 for diode.
  for (int i = 9; i < 14; i++) {
    pinMode(i, OUTPUT);
  }

  randomSeed(GetTemp()); // Best randomness available w/o RNG
  Serial.begin(9600); // Debugging
}

const unsigned int NUM_OF_STATES = 6;
unsigned int myState = 0;
bool skip = false; // skip delays
void loop() {
  switch (myState) {
    case 0: blinkDiode(); break;
    case 1: soundActivated(); break;
    case 2: blinkAll(); break;
    case 3: blinkAllFade(10); break;
    case 4: blinkAllFade(1); break;
    case 5: blinkAllRandom(); break;
    default:
      Serial.println("Unknown state"); delay(1000);
  }
  skip = false;
}

//////////  BLINK PATTERNS  //////////

int soundMax = 0;
void soundActivated(void) {
  int val = analogRead(soundPin);
  soundMax = (val > soundMax) ? val : (soundMax - 1);

  int led = 11; // green
  if (soundMax < 27) {
    led = 10; // blue
  } else if (soundMax > 29) {
    led = 9; // red
  }
  digitalWrite(led, HIGH);
  for (int i = 9; i < 12; i++) {
    if (i != led) digitalWrite(i, LOW);
  }
  if (!skip && led == 9) delay(20);
}

void blinkDiode(void) {
  digitalWrite(13, HIGH);
  if (!skip) delay(500);
  digitalWrite(13, LOW);
  if (!skip) delay(500);
}

void blinkAll(void) { // R -> G -> B
  for (int led = 9; led < 12; led++) {
    digitalWrite(led, HIGH);
    if (!skip) delay(1000);
    digitalWrite(led, LOW);
  }
}

void blinkAllFade(int hastighed) { // R -> G -> B
  for (int led = 9; led < 12; led++) {
    for (int brightness = 0; brightness < 255; brightness++) {
      analogWrite(led, brightness); // turn up
      if (!skip) delay(hastighed);
    }
    for (int brightness = 255-1; brightness >= 0; --brightness) {
      analogWrite(led, brightness); // turn down
      if (!skip) delay(hastighed);
    }
  }
}

void blinkAllRandom(void) {
  int led = random(9, 12);
  int brightness = random(255);
  analogWrite(led, brightness);
  if (!skip) delay(50);
}


//////////  HELPERS and HANDLERS  //////////

unsigned int lastChange = 0; // Shared variable. Make sure every action is atomic.
void inputHandler(void) {
  noInterrupts(); // Begin critical region
  
  if (lastChange + 500 < millis()) { // 0.5 sek delay
    lastChange = millis();

    myState = (++myState % NUM_OF_STATES); // Update state
    cleanUp();
    Serial.println("New state: " + String(myState));
  }
  
  interrupts(); // End critical region
}

void cleanUp(void) {
  skip = true; // skip delays to end any running blink func.
  for (int led = 9; led < 13; led++) {
    digitalWrite(led, LOW); // samme som analogWrite(led, 0) ?
  }
}

// Credit: http://playground.arduino.cc/Main/InternalTemperatureSensor
double GetTemp(void) // Serial.println(GetTemp(),1);
{
  unsigned int wADC;
  double t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA, ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31) / 1.22;

  // The returned temperature is in degrees Celsius.
  return (t);
}
