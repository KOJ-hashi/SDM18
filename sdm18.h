#ifndef sdm18_H_
#define sdm18_H_
#include "mbed.h"
#define CRC16_POLY 0x8005
 
/////////////start data//////////////////////////////
    const int Packet_heade=0xA5;
    const int Device_number= 0x03;
    const int Device_type=0x20;
    const int Command_type_start=0x01;
    const int Reserved_bit =0x00;
    const int Data_length =0x00;
    const int Data_length2=0x00;
    const int CheckSum_start=0x02;
    const int CheckSum_start2=0x6E;
    ///////////////stop data///////////////////////////////////
    const int Command_type_stop=0x02;
    const int CheckSum_stop=0x46;
    const int CheckSum_stop2=0x6E; 
    /////////////set baud rate///////////////////////////////////
    const int Command_type_setbaudrate=0x10; 
    const int Data_length_setbaudrate=0x01;
    const int BaudRate=0x07;
    const int CRC_setbaudrate=0xBD;
    const int CRC_setbaudrate2=0x3F;
 
//上記詳細はデータシートで
 
class sdm18{
public:
    sdm18(BufferedSerial &sensor,CAN &can);
    bool setbaudrate(char baudrate);
    uint16_t calculate_crc16(char *buf, int len);
    bool getdata();//値を取得する関数、正しくとれていたらtrue
    bool startscan();//スキャンを開始する関数、正しくとれていたらtrue
    bool stopscan();//スキャンを終了する関数、正しくとれてたらtrue
    void sdm18_send();//canを送る
private:
    CANMessage _canMessage;
    BufferedSerial &_sensor;
    CAN &_can;
    char scan_recv_start[23];//センサから出力される値(始めたとき~動作時)
    char scan_recv_stop[10];//センサから出力される値(動作終了時)
    char scan_recv_setbaudrate[10];//センサから出力される値(ボードレート指定時)
    uint16_t crc_result;//実際にセンサが動作した時の合計値(理論値てきな)
    uint16_t checksum;//しっかりとセンサが正しい動作をしたかを確かめる変数
    int _a;
};
#endif
