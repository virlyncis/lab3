const int ledPin    = 13;
const int buttonPin = 2; 

volatile unsigned long blinkInterval     = 500;   // 500 мс → 1 Гц
volatile unsigned long lastInterruptTime = 0;

bool ledState = LOW;
unsigned long previousMillis = 0;

void buttonISR() {
    unsigned long currentTime = millis();

    // Антидребезг 200 мс
    if (currentTime - lastInterruptTime > 200) {
        blinkInterval = (blinkInterval == 500) ? 100 : 500;   
        lastInterruptTime = currentTime;
    }
}

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);   

    attachInterrupt(
        digitalPinToInterrupt(buttonPin),
        buttonISR,
        FALLING                   
    );
}

void loop() {
    unsigned long interval;

    noInterrupts();
    interval = blinkInterval;
    interrupts();

    if (millis() - previousMillis >= interval) {
        previousMillis = millis();
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
    }
}