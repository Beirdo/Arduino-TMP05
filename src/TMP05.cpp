#include "TMP05.h"
#include "vectoredInterrupt.h"

// Requires:  use of the system timer (in microseconds)
//            edge-triggered interrupts on inpin
//            https://github.com/GreyGnome/EnableInterrupt.git


TMP05::TMP05(uint8_t count, uint8_t base, uint8_t board, uint8_t outpin,
             uint8_t inpin) : p_count(count), p_base(base), p_board(board),
                              p_outpin(outpin), p_inpin(inpin), p_state(-1)

{
    p_modulo = count + 1;
    p_lastResults = new sensor_data_t[p_modulo];
    p_readIndex = 0;
    p_writeIndex = 0;

    digitalWrite(p_outpin, HIGH);
    pinMode(p_outpin, OUTPUT);
    pinMode(p_inpin, INPUT_PULLUP);
}

uint8_t TMP05::getReadingCount(void)
{
    return (p_writeIndex + p_modulo - p_readIndex) % p_modulo;
}

void TMP05::enableMyInterrupt(void)
{
    attachVectoredInterrupt(p_inpin, handleInterrupt, CHANGE);
}

void TMP05::disableMyInterrupt(void)
{
    detachVectoredInterrupt(p_inpin);
}

void TMP05::handleInterrupt(void)
{
    uint32_t now = micros();
    uint16_t delta = now - p_startMicros;
    uint32_t temp;
    sensor_data_t *sensor;

    // avoid divide by zero
    if (delta == 0) {
        delta = 1;
    }

    //uint8_t value = (arduinoPinState == 0 ? LOW : HIGH);
    p_startMicros = now;

    switch (p_state) {
    case 0:  // falling edge, measure the high pulse
        p_highMicros = delta;
        p_state = 1;
        break;
    case 1:  // rising edge, measure the low pulse
        p_lowMicros = delta;
        p_state = 0;

        if (getReadingCount() >= p_count - 1) {
            // No space to store it, carry on
            break;
        }

        temp = (421LL * 256LL) - (751LL * 256LL * p_highMicros / p_lowMicros);
        sensor = &p_lastResult[p_writeIndex];
        p_writeIndex = (p_writeIndex + 1) % p_modulo;
        sensor->timestamp = millis();
        sensor->boardNum = p_board;
        sensor->sensorNum = p_current_input++;
        sensor->reading = (uint8_8t)temp;

        if (p_current_input >= p_count) {
            p_state = 2;
            p_current_input = 0;
        }
        break;
    case 2:  // falling edge of the start trigger, we are done.
    default:
        p_state = -1;
        disableMyInterrupt();
        digitalWrite(p_outpin, HIGH);
        delayMicroseconds(30);
        break;
    }
}

void TMP05::startConversion(void)
{
    // Pulse the outpin low, high (>= 25ns, <= 20us), (arm interrupts),
    // low to start
    digitalWrite(p_outpin, LOW);
    delayMicroseconds(1);

    digitalWrite(p_outpin, HIGH);
    delayMicroseconds(20);

    enableMyInterrupt();
    p_state = 0;
    p_current_input = 0;
    p_startMicros = micros();
    digitalWrite(p_outpin, LOW);
}

sensor_data_t *TMP05::getReading(void)
{
    if (getReadingCount() == 0) {
        return NULL;
    }

    uint8_t index = p_readIndex;
    p_readIndex = (p_readIndex + 1) % p_modulo;
    return &p_lastResults[index];
}

bool TMP05::inactive(void)
{
    return (p_state == -1);
}

// vim:ts=4:sw=4:ai:et:si:sts=4
