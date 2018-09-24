#include "TMP05.h"

#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

// Requires:  use of the system timer (in microseconds)
//            edge-triggered interrupts on inpin
//            https://github.com/GreyGnome/EnableInterrupt.git

static void interruptHandler(void);

TMP05 *sensorChain = NULL;

TMP05::TMP05(uint8_t count, uint8_t outpin, uint8_t inpin)
{
    p_count = count;
    p_outpin = outpin;
    p_inpin = inpin;
    p_state = -1;

    uint8_t blockSize = count * sizeof(uint8_8t);
    p_lastResults = new uint8_8t[count];
    memset(p_lastResults, 0xFF, blockSize);

    digitalWrite(p_outpin, HIGH);
    pinMode(p_outpin, OUTPUT);
    pinMode(p_inpin, INPUT_PULLUP);
    sensorChain = this;
}

void TMP05::enableMyInterrupt(void)
{
    // Set the pin to an input with pullup
    pinMode(p_inpin, INPUT_PULLUP); // /!\ pinMode form THE ARDUINO CORE

    // Attach interrupt handler
    attachPinChangeInterrupt(p_inpin, interruptHandler, CHANGE);
}

void TMP05::disableMyInterrupt(void)
{
    detachPinChangeInterrupt(p_inpin);
}

static void interruptHandler(void)
{
    if (!sensorChain) {
        return;
    }
    sensorChain->handleInterrupt();
}

void TMP05::handleInterrupt(void)
{
    uint32_t now;
    uint16_t delta;
    uint32_t temp;

    now = micros();
    delta = now - p_startMicros;
    p_startMicros = now;

    switch (p_state) {
    case 0:
        p_highMicros = delta;
        p_state = 1;
        break;
    case 1:
        if (delta == 0) {
            delta = 1;
        }
        p_lowMicros = delta;
        p_state = 0;

        temp = (421LL * 256LL) - (751LL * 256LL * p_highMicros / p_lowMicros);
        p_lastResults[p_index++] = (uint8_8t)temp;
        if (p_index >= p_count) {
            p_state = 2;
            p_index = 0;
        }
        break;
    case 2:
    default:
        p_state = -1;
        disableMyInterrupt();
        digitalWrite(p_outpin, HIGH);
        delayMicroseconds(30);
        break;
    }
}

uint8_t TMP05::getState(void)
{
    return p_state;
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
    p_index = 0;
    p_startMicros = micros();
    digitalWrite(p_outpin, LOW);
}

uint8_8t TMP05::getReading(uint8_t index)
{
    if (index >= p_count || !p_lastResults) {
        return (0xFFFF);
    }
    return (p_lastResults[index]);
}

// vim:ts=4:sw=4:ai:et:si:sts=4
