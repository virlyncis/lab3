#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte rowPins[4] = {7, 6, 5, 4};
const byte colPins[4] = {11, 10, 9, 8};
const int KEY_INT_PIN = 2;             

const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

volatile unsigned long timerCount = 0;
volatile bool scanFlag = false;

ISR(TIMER1_COMPA_vect) { timerCount++; }

void startScan() { scanFlag = true; }

void setupKeyboardPins() {
    for (byte i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
        pinMode(colPins[i], INPUT_PULLUP);
    }
    pinMode(KEY_INT_PIN, INPUT_PULLUP);
}

void setupTimer1() {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);
    OCR1A = 7811;                          
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

char scanKeyboard() {
    for (byte i = 0; i < 4; i++) {
        digitalWrite(rowPins[i], LOW);
        delay(1);
        for (byte j = 0; j < 4; j++) {
            if (digitalRead(colPins[j]) == LOW) {
                digitalWrite(rowPins[i], HIGH);
                return keys[i][j];
            }
        }
        digitalWrite(rowPins[i], HIGH);
    }
    return 0;
}

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    setupKeyboardPins();
    setupTimer1();

    attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), startScan, FALLING);

    sei();

    lcd.setCursor(0, 0);
    lcd.print("System Ready");
}

void loop() {
    static unsigned long lastDisplay = 0;
    unsigned long cnt;

    noInterrupts();
    cnt = timerCount;
    interrupts();

    if (cnt != lastDisplay) {
        lastDisplay = cnt;
        lcd.setCursor(0, 0);
        lcd.print("Count: ");
        lcd.print(cnt);
        lcd.print("        ");
    }

    static unsigned long lastScan = 0;
    bool needScan = scanFlag || (millis() - lastScan > 10);

    if (needScan) {
        detachInterrupt(digitalPinToInterrupt(KEY_INT_PIN));

        noInterrupts();
        scanFlag = false;
        interrupts();

        char pressedKey = scanKeyboard();

        if (pressedKey != 0) {
            lcd.setCursor(0, 1);
            lcd.print("Key: ");
            lcd.print(pressedKey);
            lcd.print("        ");
            Serial.print("Key: ");
            Serial.println(pressedKey);

            if (pressedKey == 'A') {
                noInterrupts();
                timerCount = 0;
                interrupts();
            }
        }

        attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), startScan, FALLING);
        lastScan = millis();
    }
}