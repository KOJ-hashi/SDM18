#include "sdm18.h"

sdm18::sdm18(BufferedSerial &sensor, CAN &can) : _sensor(sensor), _can(can) {
    _can.frequency(1000000); // 1MHz
}

uint16_t sdm18::calculate_crc16(char *buf, int len) {
    uint16_t crc = 0xFFFF;
    for(int i = 0; i < len; i++ ) {
        crc ^= buf[i];
        for(int bit = 0; bit < 8; bit++ ) {
            if( crc & 0x0001 ) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool sdm18::startscan(){
    char cmd_start[9] = {0xA5, 0x03, 0x20, 0x01, 0x00, 0x00, 0x00, 0x02, 0x6E};
    _sensor.write(cmd_start, sizeof(cmd_start));
    return true;
}

bool sdm18::stopscan(){
    char cmd_stop[9] = {0xA5, 0x03, 0x20, 0x02, 0x00, 0x00, 0x00, 0x46, 0x6E};
    _sensor.write(cmd_stop, sizeof(cmd_stop));
    return true;
}

bool sdm18::setbaudrate(char baudrate) {
    char setbaudrate_cmd[10] = {0xA5, 0x03, 0x20, 0x10, 0x00, 0x00, 0x01, baudrate, 0xBD, 0x3F};
    _sensor.write(setbaudrate_cmd, sizeof(setbaudrate_cmd));
    return true;
}

// 安定版データ取得：ヘッダを確実に捕らえてCRCを検証
bool sdm18::getdata(){
    if (_sensor.readable()) {
        char header = 0;
        if (_sensor.read(&header, 1) == 1 && header == 0xA5) {
            scan_recv_start[0] = 0xA5;
            size_t read_bytes = 0;
            int timeout = 0;
            
            while (read_bytes < 22 && timeout < 15) {
                if (_sensor.readable()) {
                    ssize_t n = _sensor.read(&scan_recv_start[1 + read_bytes], 22 - read_bytes);
                    if (n > 0) {
                        read_bytes += n;
                    }
                } else {
                    ThisThread::sleep_for(1ms);
                    timeout++;
                }
            }

            if (read_bytes == 22) {
                checksum = (static_cast<uint8_t>(scan_recv_start[21]) << 8) | static_cast<uint8_t>(scan_recv_start[22]);
                crc_result = calculate_crc16(scan_recv_start, 21);
                
                if (checksum == crc_result && checksum != 65535) {
                    return true;
                }
            }
        }
    }
    return false;
}

void sdm18::sdm18_send() {
    _canMessage.len = 2;
    _canMessage.id = 0x701; // F446REが待ち受けるID
    _canMessage.data[0] = scan_recv_start[13]; // 距離下位
    _canMessage.data[1] = scan_recv_start[14]; // 距離上位
    _can.write(_canMessage);
}
