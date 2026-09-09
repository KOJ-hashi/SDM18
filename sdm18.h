#ifndef INCLUDED_SDM18_H
#define INCLUDED_SDM18_H

#include "mbed.h"

class sdm18 {
public:
    // main側で定義したピンや通信インスタンスを受け取るコンストラクタ
    sdm18(BufferedSerial& sensor, CAN& can, DigitalOut& led_scan, DigitalOut& led_id, DigitalOut& led_spare, DigitalOut& led_boot, uint32_t id);
    
    void init();
    void process();

    uint16_t get_latest_distance() const;
    uint32_t get_data_interval() const;
    bool is_data_received() const;

private:
    uint16_t calculate_crc16(char *buf, int len);
    void update_id_led();
    const uint32_t TIMEOUT = 40;

    // 外部（main）から渡されたオブジェクトへの参照を保持
    BufferedSerial& _sensor;
    CAN& _can;
    DigitalOut& led_scan;
    DigitalOut& led_id;
    DigitalOut& led_spare;
    DigitalOut& led_boot;

    const uint32_t _id;
    static const uint16_t _crc16_table[256];

    char buf[23];
    uint16_t _latest_distance;
    bool data_received;
    uint32_t last_data_ms;
    uint32_t data_interval_ms;
    bool first_data;

    Timer blink_timer;
    bool blink_init;
    int state;
    int current_count;
    int target_blinks;
};

#endif
