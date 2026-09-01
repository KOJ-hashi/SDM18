#ifndef INCLUDED_SDM18_H
#define INCLUDED_SDM18_H

#include"mbed.h

class sdm18{
public:

sdm18(BufferedSerial& sensor_stream, CAN& can_bus, DigitalOut& led_scan,DigitalOut& led_id,DigitalOut& led_id_spare, DigitalOut& led_boot, uint32_t id);

void init();
void prpcess();

uint16_t get_latest_distance() const;
uint32_t get_data_interval() const;
bool is_data_received() const;

private:

uint16_t calculate_crc16(char *buf, int len);
void update_id_led();

BufferedSerial& _sensor;
CAN& _can;
DigitalOut& _led_id;
DigitalOut& _led_scan;
DigitalOut& _led_spare;
DigitalOut& _led_boot;

uint32_t _id;
static const uint16_t _crc16_table[256];

uint32_t TIMEOUT = 40;

char buf[23];
uint16_t latest_distance = 0;
bool _data_received;
uint32_t _last_data_ms = 0;
uint32_t _data_interval_ms = 0;
bool _first_data = true;


Timer _blink_timer;
bool _blink_init;
int _state;
int _current_count;
int _target_blinks;
};

#endif
