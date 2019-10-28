/////////////////////////
//Author: philippmay3r///
//Website: www.may3r.at//
//Date: 24.10.2019///////
//Project: easy blinds///
/////////////////////////

////////////////////////////////////
// __  __    _ __   _______ ____////
//|  \/  |  / \\ \ / |___ /|  _ \///
//| |\/| | / _ \\ V /  |_ \| |_) |//
//| |  | |/ ___ \| |  ___) |  _ <///
//|_|  |_/_/   \_|_| |____/|_| \_\//
////////////////////////////////////


//////////
//Config//
//////////

//here you can configure your project
//
//enter your start pin, I recommend 3
//maxium possible blinds: 12 with one ATmega2560

const int startPins = 3; // start only every 4 pins like, 0, 3, 7,... else it is your problem ;)
const int maxPins = 51;

const unsigned long onTimeRollo = 60000;
const unsigned long pushedTimeBtn = 500;

//////////////////////
//for inverted Logic//
//////////////////////

//if you use Pull Down resistor:
// bool inverted = false;

//if you use Pull Up resistor:
// bool inverted = true;

bool inverted = true;

///////////////////
//Internal PULLUP//
///////////////////

//if you want to use your onboard pull up resistor
//you can set here "true" else use "false" if you have external one
bool useInternalResistor = false;

/////////
//DEBUG//
/////////

//for better analysis set debug to "true"
bool debug = false;

////////////////////
//Global Variables//
////////////////////

//do not change these values

const unsigned long maxVal = 4294967290;
bool disable_MillisOverflow = false;
const int possibleJalousie = maxPins / 4;

bool inputReadingButtonState[possibleJalousie][2];
bool inputButtonState[possibleJalousie][2];
bool lastInputButtonState[possibleJalousie][2];
bool outputEnabled[possibleJalousie][2];

unsigned long changedInputs[possibleJalousie][2];
unsigned long changedOutputs[possibleJalousie][2];


void initMode() {
  initPinMode();
}

void writeOutputs() {
  if (debug) {
    Serial.println("- - start writting outputs - -");
  }
  int writtenPin = 0;
  for (int i = 1; i <= possibleJalousie; i++) {
    for (int j = 0; j < 2; j++) {
      writtenPin = (((i * 4) + j) + startPins) - 1;
      if (debug) {
        String output = (String)"Output: " + (writtenPin) + (String)" = " + outputEnabled[i - 1][j];
        Serial.println(output);
      }

      if (millis() - changedOutputs[i - 1][j] > onTimeRollo) {
        outputEnabled[i - 1][j] = false;
        changedOutputs[i - 1][j] = 0;
      }
      digitalWrite(writtenPin, inverted ? !outputEnabled[i - 1][j] : outputEnabled[i - 1][j]);
    }
  }
  if (debug) {
    Serial.println("- - stop writting outputs - -");
  }
}

void readButtons() {
  if (debug) {
    Serial.println("- - start reading buttons - -");
  }
  String output = "";
  for (int i = startPins; i <= maxPins; i++) {
    if (i % 4 == 0) {
      for (int k = 1; k <= 4; k++) {
        int currentPosition = i + k - 1;
        if (currentPosition <= maxPins) {
          switch (k) {
            case 1:
            case 2:
              //get Input and save the value for current state
              inputReadingButtonState[(i - startPins) / 4][k - 1] = inverted ? !digitalRead(currentPosition) : digitalRead(currentPosition);

              if (debug) {
                String output = (String)"Input: " + (currentPosition) + (String)" = " + inputReadingButtonState[(i - startPins) / 4][k - 1];
                Serial.println(output);
              }


              if (inputReadingButtonState[(i - startPins) / 4][k - 1] == true) {
                if (inputReadingButtonState[(i - startPins) / 4][k - 1] != lastInputButtonState[(i - startPins) / 4][k - 1]) {
                  changedInputs[(i - startPins) / 4][k - 1] = millis();
                }
              } else {
                changedInputs[(i - startPins) / 4][k - 1] = 0;
              }

              if ((millis() - changedInputs[(i - startPins) / 4][k - 1]) > pushedTimeBtn) {
                if (inputReadingButtonState[(i - startPins) / 4][k - 1] != inputButtonState[(i - startPins) / 4][k - 1]) {
                  inputButtonState[(i - startPins) / 4][k - 1] = inputReadingButtonState[(i - startPins) / 4][k - 1];
                  switch (k) {
                    case 1:
                      if (!outputEnabled[(i - startPins) / 4][k - 1] && inputButtonState[(i - startPins) / 4][k - 1]) {
                        if (outputEnabled[(i - startPins) / 4][k]) {
                          outputEnabled[(i - startPins) / 4][k - 1] = false;
                          outputEnabled[(i - startPins) / 4][k] = false;
                        } else {
                          outputEnabled[(i - startPins) / 4][k - 1] = true;
                          changedOutputs[(i - startPins) / 4][k - 1] = millis();
                        }
                      }
                      break;

                    case 2:
                      if (!outputEnabled[(i - startPins) / 4][k - 1] && inputButtonState[(i - startPins) / 4][k - 1]) {
                        if (outputEnabled[(i - startPins) / 4][k - 2]) {
                          outputEnabled[(i - startPins) / 4][k - 1] = false;
                          outputEnabled[(i - startPins) / 4][k - 2] = false;
                        } else {
                          outputEnabled[(i - startPins) / 4][k - 1] = true;
                          changedOutputs[(i - startPins) / 4][k - 1] = millis();
                        }
                      }
                      break;
                  }
                }
              }
              lastInputButtonState[(i - startPins) / 4][k - 1] = inputReadingButtonState[(i - startPins) / 4][k - 1];
              break;
          }
        }
      }
    }
  }
  if (debug) {
    Serial.println("- - stop reading buttons - -");
  }
}

void checkMillis() {
  if ((maxVal - onTimeRollo <= millis()) || (maxVal - pushedTimeBtn <= millis())) {
    disable_MillisOverflow = true;
  }
  else {
    disable_MillisOverflow = false;
  }
}

void initInput(int pin) {
  pinMode(pin, useInternalResistor ? INPUT_PULLUP : INPUT);
}

void initOutput(int pin) {
  pinMode(pin, OUTPUT);
}

void initPinMode() {
  if (debug) {
    Serial.println("- - start initiating buttons - -");
  }
  String output = "";
  int currentPosition = 0;

  for (int i = startPins; i <= maxPins; i++) {
    if (i % 4 == 0) {
      //Serial.println(i);
      for (int k = 1; k <= 4; k++) {
        currentPosition = i + k - 1;
        if (currentPosition <= maxPins) {
          switch (k) {
            case 1:
            case 2:
              initInput(currentPosition);
              if (debug) {
                output = (String)"Input: " + currentPosition;
                Serial.println(output);
              }
              break;
            case 3:
            case 4:
              initOutput(currentPosition);
              if (debug) {
                output = (String)"Output: " + currentPosition;
                Serial.println(output);
              }
              break;
          }
        }
      }
    }
  }

  if (debug) {
    Serial.println("- - stop initiating buttons - -");
  }
}

/////////
//Setup//
/////////

void setup() {
  Serial.begin(9600);
  initMode();
}

void loop() {
  checkMillis();
  if (!disable_MillisOverflow) {
    writeOutputs();
    readButtons();
  }
}
