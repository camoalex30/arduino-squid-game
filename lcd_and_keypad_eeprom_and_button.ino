
// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

//button setup-------------------------
byte buttonPins[4] = {30, 31, 32, 33};
//EEPROM SETUP --------------------------

const int INITIAL_VERSION = 110;
struct AppState {
  uint8_t version = INITIAL_VERSION;
  char text[14] = "______________\0";
  uint8_t nextChar = 1;
};

const int EE_ADDR = 0; // start of EEPROM
AppState state;

unsigned long lastWriteMs = 0;

//EEPROM FUNCTIONS---------------------

void loadState() {
  EEPROM.get(EE_ADDR, state);
  if (state.version != INITIAL_VERSION) { // first run or incompatible
    state = AppState(); // defaults
    EEPROM.put(EE_ADDR, state); // initialize EEPROM
  }
  state.text[14] = '\0';
}

void saveStateIfChanged(const AppState& before) {
  //if (memcmp(&before, &state, sizeof(AppState)) != 0) {
    EEPROM.put(EE_ADDR, state); // only touches changed bytes
  //}
}

//END OF EEPROM FUNCTIONS---------------------






const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'d', 'c', 'b', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 41, en = 43, d4 = 45, d5 = 47, d6 = 49 , d7 = 51;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  //more button stuff-------------
  for (int i=0; i<4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");

  // read eeprom, write default values if eeprom empty
  loadState();
  lcd.setCursor(0, 1);
  lcd.print(state.text);
  
}




void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //more button-------------------
  for (int i=0; i<4; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      lcd.print(buttonPins[i]);
    }
  }
  lcd.setCursor(0, 1);
  char customKey = customKeypad.getKey();

  
  if (customKey) {
    
    Serial.println(customKey);
    
    AppState before = state;
    state.text[state.nextChar] = customKey;
    state.nextChar++;
    if(state.nextChar == 14) {
      state.nextChar = 1;
    }
    
    Serial.println(state.text);
    lcd.print(state.text);

    // throttle: avoid hammering EEPROM (100k writes/cell typical)
    if (millis() - lastWriteMs > 500) {
      saveStateIfChanged(before);
      lastWriteMs = millis();
    }
    
  }
}
