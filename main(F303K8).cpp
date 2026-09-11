#include "mbed.h"

//使うピン番の設定
BufferedSerial sensor(PA_9,PA_10,921600);
CAN can(PA_11,PA_12,1000000);

//LED宣言
DigitalOut led_scan(PA_0);
DigitalOut led(PB_5);
DigitalOut led_id(PB_4);
DigitalOut led_boot(PA_1);


const uint32_t SDM_ID = 0x702;//SDMのCANのID。0x701~
const uint32_t R1_EXTEN = 100;//SDMから車体最前部の角管の鉛直向きの延長線までの距離[mm]
const int SDM_OFFSET = -140;//SDMの補正値　多分30で大丈夫
const uint32_t MAX_LIMIT = 10000;

//SDMに開始のためのコマンドを送る
const uint8_t cmd_start[]={
    0xA5,
    0x03,
    0x20,
    0x01,
    0x00,
    0x00,
    0x00,
    0x02,
    0x6E
};

//上位マイコン(F446RE)にCANで送るための関数
void send_can_data(uint16_t distance){
    char can_data[2];

    can_data[0] = static_cast<char>(distance & 0xFF);
    can_data[1] = static_cast<char>((distance >>8)& 0xFF);

    can.write(CANMessage(SDM_ID,can_data,2));
}


//CRC16の処理系統
uint16_t calculate_crc16(uint8_t *buf, int len){
    uint16_t crc = 0xFFFF;
    for(int i = 0;i < len; i++){
        crc ^=buf[i];
        for (int bit = 0;bit < 8; bit ++){
            if(crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc;
}


//SDMの書き込んだIDを確認するLED
void id_led() {
    static Timer blink_timer;
    static bool blink_init = false;
    static int state = 0; // 0: 点滅中, 1: 待機中
    static int current_count = 0;
    static int target_blinks = static_cast<int>(SDM_ID & 0x0F);

    if (!blink_init) {
        blink_timer.start();
        blink_init = true;
    }

    if (state == 0) {
        if (blink_timer.elapsed_time() >= 200ms) {
            blink_timer.reset();
            if (led_id == 0) {
                led_id = 1;
            } else {
                led_id = 0;
                current_count++;
                if (current_count >= target_blinks) {
                    state = 1;
                    current_count = 0;
                }
            }
        }
    } else {
        if (blink_timer.elapsed_time() >= 1000ms) {
            blink_timer.reset();
            led_id = 0;
            state = 0;
        }
    }
}

//main
int main(){
    led_boot = 1;
    led_scan = 0;
    led_id = 0;
    led = 0;

    ThisThread::sleep_for(500ms);//SDMの起動まで500ms待つ

    sensor.write(reinterpret_cast<const char *>(cmd_start),sizeof(cmd_start));
    uint8_t prev = 0;
    
    while(true){
        id_led();
        uint8_t data;
         if (sensor.read(&data, 1) != 1){
            continue;
            }
        if(prev == 0xA5 && data == 0x20){
            uint8_t buf[18];
            int received = 0;

            while(received < 18){
                ssize_t n = sensor.read(&buf[received],18-received);
                if(n > 0){
                    received += n;
                }
            }
            uint16_t crc =calculate_crc16(buf, 18);
            uint16_t distance = static_cast<uint16_t>(buf[6]) | (static_cast<uint16_t>(buf[7]) << 8);
            if(distance != 0xFFFF){
                const int OFFSET = R1_EXTEN + SDM_OFFSET;
                distance +=OFFSET;//補正
                if(distance > MAX_LIMIT){
                    continue;
                }
                send_can_data(static_cast<uint16_t>(distance));//上位マイコンにデータを送る
                led_scan =!led_scan;
            }
        }
        prev = data;
    }
}
