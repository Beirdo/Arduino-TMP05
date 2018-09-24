#ifndef __TMP05_h__
#define __TMP05_h__

#include <Arduino.h>
#include <inttypes.h>

typedef uint16_t uint8_8t;

class TMP05 {
public:
    TMP05(uint8_t count, uint8_t outpin, uint8_t inpin);
    void startConversion(void);
    uint8_8t getReading(uint8_t index);
    uint8_t getState(void);
    void handleInterrupt(void);

protected:
    void enableMyInterrupt(void);
    void disableMyInterrupt(void);

private:
    uint8_t p_count;
    uint8_t p_outpin;
    uint8_t p_inpin;

    int8_t p_state;
    uint8_t p_index;

    uint32_t p_startMicros;
    uint16_t p_highMicros;
    uint16_t p_lowMicros;

    uint8_8t *p_lastResults;
};

#endif

// vim:ts=4:sw=4:ai:et:si:sts=4
