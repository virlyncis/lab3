const byte rowPins[4] = {7, 6, 5, 4};
const byte colPins[4] = {11, 10, 9, 8};

const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

volatile byte currentRow = 3;
volatile char pressedKey = 0;
volatile bool keyReady = false;
volatile bool keyLatched = false;
volatile bool anyKeyInScan = false;

void setup() {
    Serial.begin(9600);

    for (byte i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
        pinMode(colPins[i], INPUT_PULLUP);
    }

    noInterrupts();

    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0  = 0;
    TCCR0A |= (1 << WGM01);                // CTC
    OCR0A   = 249;                         // 1 мс при N = 64
    TIMSK0 |= (1 << OCIE0A);               // прерывание по совпадению A
    TCCR0B |= (1 << CS01) | (1 << CS00);   // предделитель 64

    interrupts();
}

ISR(TIMER0_COMPA_vect) {
    digitalWrite(rowPins[currentRow], HIGH);       // гасим прошлую строку
    currentRow = (currentRow + 1) & 0x03;
    digitalWrite(rowPins[currentRow], LOW);        // активируем текущую

    for (byte col = 0; col < 4; col++) {
        if (digitalRead(colPins[col]) == LOW) {
            anyKeyInScan = true;
            if (!keyLatched) {
                pressedKey = keys[currentRow][col];
                keyReady = true;
                keyLatched = true;
            }
        }
    }

    if (currentRow == 3) {                         // конец полного прохода
        if (!anyKeyInScan) keyLatched = false;
        anyKeyInScan = false;
    }
}

void loop() {
    if (keyReady) {
        char key;
        noInterrupts();
        key = pressedKey;
        keyReady = false;
        interrupts();

        Serial.print("Нажата клавиша: ");
        Serial.println(key);
    }
}