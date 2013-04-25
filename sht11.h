#include "pin.h"
#include <stdint.h>

class SHT11
{
public:
    SHT11(DigitalInOut const& data, DigitalOut const& sck);

    /* Returns temperature in 0.01°C steps.
     * Example 23.45°C is 2345*/
    int16_t temperature();

    uint16_t humidity();
protected:
    DigitalInOut const data_;
    DigitalOut const sck_;
    void start();
    bool send_byte(uint_fast8_t b);
    uint_fast8_t read_byte(bool ack);
};
