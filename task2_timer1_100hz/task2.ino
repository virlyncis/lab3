const int ledPin = 13;   // встроенный LED

void setup() {
    pinMode(ledPin, OUTPUT);

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM12);

    // 3. Значение сравнения для 100 Гц при N = 64
    OCR1A = 2499;

    // 4. Разрешаем прерывание по совпадению A
    TIMSK1 |= (1 << OCIE1A);

    // 5. Запускаем таймер
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // 6. Глобальные прерывания
    sei();
}

// ISR вызывается каждые 10 мс (100 Гц)
ISR(TIMER1_COMPA_vect) {
    static bool ledState = false;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
}

void loop() {
}