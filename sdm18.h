#ifndef SDM18_H
#define SDM18_H
#include "mbed.h"

const uint32_t SDM_OFFSET = -170;//個体自身にかける補正
const uint32_t R1_EXTEN = 100;//対象SDMから最前部角管の鉛直方向の延長線上までの距離
const uint32_t MAX_LIMIT = 10000;

const uint32_t OFFSET =SDM_OFFSET + R1_EXTEN;//ロボット最前部を0とする補正

void SDM_ID(int _sdm_id);
// 上位マイコン(F446RE)にCANで送るための関数
void send_can_data(uint16_t distance);

// SDMの書き込んだIDを確認するLED
void id_led();

// センサーの初期化とメインループ処理を行う関数
void sdm_all();

#endif
