#include "sht11.h"
#include "delay.h"
#include "debug.h"


#define SHT11_setup_delay() delay_us(100);
//nop()
#define SHT11_clock_delay() delay_us(100);
//nop(); nop(); nop();
#define SHT11_RESET         0x1e
#define SHT11_TEMP          0x03
#define SHT11_HUMIDITIY     0x05
#define SHT11_R_STATUS      0x07
#define SHT11_W_STATUS      0x06
#define SHT11_RESET_TIME_MS 11
#define SHT11_MAX_READOUT_TIME 320e-3
#define SHT11_TEMPERATURE_OFFSET -3965 //-39.65 @ 3.3V

SHT11::SHT11(DigitalInOut const& data, DigitalOut const& sck)
: data_(data), sck_(sck)
{
    data_.mode(OpenDrain);
    data_ = 1;
    sck_ = 0;
    SHT11_setup_delay();
    /* Interface reset */
    for (int i=0; i<9; i++)
    {
        sck_ = 1;
        SHT11_clock_delay();
        sck_ = 0;
        SHT11_clock_delay();
    }
    start();
    send_byte(SHT11_RESET);
    delay_ms(SHT11_RESET_TIME_MS);
}



int16_t SHT11::temperature()
{
    uint16_t result;
    start();
    send_byte(SHT11_TEMP);
    //Wait for data ready
    for (uint32_t i=0; i<(uint32_t)(MCLK*SHT11_MAX_READOUT_TIME); i++)
    {
        if (!data_) break;
    }
    result = read_byte(true)<<8;
    result |= read_byte(false);
    result += SHT11_TEMPERATURE_OFFSET;
    return result;
}

uint16_t SHT11::humidity()
{
}

void SHT11::start()
{
    //Start sequence
    SHT11_setup_delay();
    sck_ = 1;
    SHT11_setup_delay();
    data_ = 0;
    SHT11_setup_delay();
    sck_ = 0;
    SHT11_clock_delay();
    sck_ = 1;
    SHT11_setup_delay();
    data_ = 1;
    SHT11_setup_delay();
    sck_ = 0;
    delay_ms(10);
}

bool SHT11::send_byte(uint_fast8_t b)
{
    for (int i=0; i<8; i++)
    {
        uint8_t tmp = b & 0x80;
        data_ = b & 0x80;
        SHT11_setup_delay();
        sck_ = 1;
        SHT11_clock_delay();
        sck_ = 0;
        SHT11_setup_delay();
        b <<= 1;
    }
    data_ = 1;
    SHT11_setup_delay();
    sck_ = 1; //read ack
    SHT11_setup_delay();
    uint8_t error = data_;
    dbg_write_u8(&error, 1);
    sck_ = 0;
    SHT11_setup_delay();
    delay_ms(10);
}

uint_fast8_t SHT11::read_byte(bool ack)
{
    uint_fast8_t result = 0;
    data_ = 1;
    for (int i=0; i<8; i++)
    {
        sck_ = 1;
        result <<= 1;
        if (data_) result |= 1;
        sck_ = 0;
    }
    data_ = ack ? 0 : 1; //Pull data line low for ACK
    SHT11_setup_delay();
    sck_ = 1;
    SHT11_clock_delay();
    sck_ = 0;
    SHT11_setup_delay();
    data_ = 1;
    return result;
}


