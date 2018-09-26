#ifndef __TMP05_h__
#define __TMP05_h__

#include <Arduino.h>
#include <inttypes.h>

#ifndef uint8_8t
#define uint8_8t uint16_t
#endif

typedef struct {
    uint32_t timestamp; // 4 bytes, ms relative to boot (wraps every 49.71 days)
    uint8_t boardNum;   // 1 byte
    uint8_t sensorNum;  // 1 byte
    uint8_8t reading;   // 2 bytes, temperature in C * 256
} sensor_data_t;

class TMP05 {
public:
    TMP05(uint8_t count, uint8_t base, uint8_t board, uint8_t outpin,
          uint8_t inpin);
    void startConversion(void);
    uint8_t getReadingCount(void);
    sensor_data_t *getReading(void);
    bool inactive(void);

protected:
    void handleInterrupt(void);
    void enableMyInterrupt(void);
    void disableMyInterrupt(void);
private:
    uint8_t p_count;
    uint8_t p_modulo;
    uint8_t p_base;
    uint8_t p_board;
    uint8_t p_outpin;
    uint8_t p_inpin;

    volatile int8_t p_state;
    volatile uint8_t p_current_input;

    uint32_t p_startMicros;
    uint16_t p_highMicros;
    uint16_t p_lowMicros;

    sensor_data_t *p_lastResults;
    volatile uint8_t p_writeIndex;
    volatile uint8_t p_readIndex;
};

#endif

// vim:ts=4:sw=4:ai:et:si:sts=4
