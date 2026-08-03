#ifndef DS1307_H
#define DS1307_H

#include <cstdint>

typedef struct
{
    uint8_t s;
    uint8_t m;
    uint8_t h;
    uint8_t W;
    uint8_t D;
    uint8_t M;
    uint8_t Y;
} DateTime;

uint8_t bcd_to_dec(uint8_t val);
uint8_t dec_to_bcd(uint8_t val);

void ds1307_set_time(uint8_t sec, uint8_t min, uint8_t hour,
                     uint8_t dayOfWeek, uint8_t date, uint8_t month, uint8_t year);
void ds1307_read_time(DateTime &dt);

#endif
