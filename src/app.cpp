/**
 ******************************************************************************
 ** ファイル名 : app.c
 **
 ** 概要 : 2輪倒立振子ライントレースロボットのTOPPERS/HRP2用Cサンプルプログラム
 **
 ** 注記 : sample_c4 (sample_c3にBluetooth通信リモートスタート機能を追加)
 ******************************************************************************
 **/

#include "ev3api.h"
#include "app.h"
#include "balancer.h"
#include "util.h"
#include "setting.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/**
 * センサー、モーターの接続を定義します
 */
static const sensor_port_t
    touch_sensor    = EV3_PORT_1,
    sonar_sensor    = EV3_PORT_2,
    color_sensor    = EV3_PORT_3,
    gyro_sensor     = EV3_PORT_4;

static const motor_port_t
    left_motor      = EV3_PORT_C,
    right_motor     = EV3_PORT_B,
    tail_motor      = EV3_PORT_A;

static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

/* 関数プロトタイプ宣言 */
static int sonar_alert(void);
static void tail_control(signed int angle);
void setLineTracePwm(const int& brightness_, signed char& forward_, signed char& turn_);
    
#include "BalancerCpp.h"        // <1>
Balancer balancer;              // <1>

/* メインタスク */
void main_task(intptr_t unused)
{
    signed char forward;      /* 前後進命令 */
    signed char turn;         /* 旋回命令 */
    signed char pwm_L, pwm_R; /* 左右モータPWM出力 */
    char msg[32];
    int brightness; /* 地面の明るさ */

    /* センサー入力ポートの設定 */
    ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);
    ev3_color_sensor_get_reflect(color_sensor); /* 反射率モード */
    ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
    ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
    /* モーター出力ポートの設定 */
    ev3_motor_config(left_motor, LARGE_MOTOR);
    ev3_motor_config(right_motor, LARGE_MOTOR);
    ev3_motor_config(tail_motor, LARGE_MOTOR);
    ev3_motor_reset_counts(tail_motor);

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);
    

#if 1 /* 片山先生がいなければ0にする */
    /* 片山先生のお顔 */
    memfile_t memfile_img;
    ev3_memfile_load("/ev3rt/res/katayama.bmp", &memfile_img);
    memfile_t memfile_wav;
    ev3_memfile_load("/ev3rt/res/nogi.wav", &memfile_wav);
    //ev3_memfile_load("/ev3rt/res/dorakue.wav", &memfile_wav);
    image_t image;
    ev3_image_load(&memfile_img, &image);
    ev3_lcd_draw_image(&image, 0, 0);
    ev3_speaker_set_volume(10);
    ev3_speaker_play_file(&memfile_wav, SOUND_MANUAL_STOP);
#endif

    ev3_led_set_color(LED_ORANGE); /* 初期化完了通知 */

    int buff = 0;
    /* スタート待機 */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP + buff); /* 完全停止用角度に制御 */
        brightness = ev3_color_sensor_get_reflect(color_sensor); /* 光センサの値取得 */           

        /* LCDにしっぽの角度と光センサの値を出力する */
        sprintf(msg, "Tail_Angle: %d, Brightness: %d", TAIL_ANGLE_STAND_UP + buff, brightness);
        msg_f(msg, 0);

        if(ev3_button_is_pressed(LEFT_BUTTON)){
            buff--;
            tslp_tsk(500); /* 500msecウェイト */
        }

        if(ev3_button_is_pressed(RIGHT_BUTTON)){
            buff++;
            tslp_tsk(500); /* 500msecウェイト */            
        }

        if (bt_cmd == 1)
        {
            break; /* リモートスタート */
        }
        if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
        {
            break; /* タッチセンサが押された */
        }
        tslp_tsk(10); /* 10msecウェイト */
    }

    /* 走行モーターエンコーダーリセット */
    ev3_motor_reset_counts(left_motor);
    ev3_motor_reset_counts(right_motor);

    /* ジャイロセンサーリセット */
    ev3_gyro_sensor_reset(gyro_sensor);
    balancer.init(GYRO_OFFSET);                // <1>

    ev3_led_set_color(LED_GREEN); /* スタート通知 */

    /**
    * Main loop for the self-balance control algorithm
    */
    bool isSonerAlert = false;    
    while(1)
    {
        int32_t motor_ang_l, motor_ang_r;
        int gyro, volt;
        if (ev3_button_is_pressed(BACK_BUTTON)) break;

        tail_control(TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */

        if (sonar_alert() == 1) /* 障害物検知 */
        {
            //if(isSonerAlert == false)ev3_speaker_play_tone (NOTE_FS6, 100);
            isSonerAlert = true;
            forward = turn = 0; /* 障害物を検知したら停止 */
        }
        else
        {
            brightness = ev3_color_sensor_get_reflect(color_sensor);            

            setLineTracePwm(brightness, forward, turn); /* 前後進命令, 旋回命令の設定 */

            //if(isSonerAlert == true)ev3_speaker_play_tone (NOTE_GS6, 100);            
            isSonerAlert = false;
            /* LCDに光センサの明るさ値を出力する */
            //sprintf(msg, "Brightness: %d", brightness);
            //msg_f(msg, 3);  
        }

        /* 倒立振子制御API に渡すパラメータを取得する */
        motor_ang_l = ev3_motor_get_counts(left_motor);
        motor_ang_r = ev3_motor_get_counts(right_motor);
        gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
        volt = ev3_battery_voltage_mV();

        /* 倒立振子制御APIを呼び出し、倒立走行するための */
        /* 左右モータ出力値を得る */
        balancer.setCommand(forward, turn);   // <1>
        balancer.update(gyro, motor_ang_r, motor_ang_l, volt); // <2>
        pwm_L = balancer.getPwmRight();       // <3>
        pwm_R = balancer.getPwmLeft();        // <3>

        /* EV3ではモーター停止時のブレーキ設定が事前にできないため */
        /* 出力0時に、その都度設定する */
        if (pwm_L == 0)
        {
             ev3_motor_stop(left_motor, true);
        }
        else
        {
            ev3_motor_set_power(left_motor, (int)pwm_L);
        }

        if (pwm_R == 0)
        {
             ev3_motor_stop(right_motor, true);
        }
        else
        {
            ev3_motor_set_power(right_motor, (int)pwm_R);
        }

        tslp_tsk(4); /* 4msec周期起動 */
    }
    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// 関数名 : sonar_alert
// 引数 : 無し
// 返り値 : 1(障害物あり)/0(障害物無し)
// 概要 : 超音波センサによる障害物検知
//*****************************************************************************
static int sonar_alert(void)
{
    static unsigned int counter = 0;
    static int alert = 0;

    signed int distance;
    char msg[32];

    if (++counter == 40/4) /* 約40msec周期毎に障害物検知  */
    {
        /*
         * 超音波センサによる距離測定周期は、超音波の減衰特性に依存します。
         * NXTの場合は、40msec周期程度が経験上の最短測定周期です。
         * EV3の場合は、要確認
         */
        distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
        sprintf(msg, "Soner: %d", distance);
        //msg_f(msg, 2);
        
        if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
        {
            alert = 1; /* 障害物を検知 */
        }
        else
        {
            alert = 0; /* 障害物無し */
        }
        counter = 0;
    }

    return alert;
}

//*****************************************************************************
// 関数名 : tail_control
// 引数 : angle (モータ目標角度[度])
// 返り値 : 無し
// 概要 : 走行体完全停止用モータの角度制御
//*****************************************************************************
static void tail_control(signed int angle)
{
    float pwm = (float)(angle - ev3_motor_get_counts(tail_motor))*P_GAIN; /* 比例制御 */
    /* PWM出力飽和処理 */
    if (pwm > PWM_ABS_MAX)
    {
        pwm = PWM_ABS_MAX;
    }
    else if (pwm < -PWM_ABS_MAX)
    {
        pwm = -PWM_ABS_MAX;
    }

    if (pwm == 0)
    {
        ev3_motor_stop(tail_motor, true);
    }
    else
    {
        ev3_motor_set_power(tail_motor, (signed char)pwm);
    }
}

//*****************************************************************************
// 関数名 : bt_task
// 引数 : unused
// 返り値 : なし
// 概要 : Bluetooth通信によるリモートスタート。 Tera Termなどのターミナルソフトから、
//       ASCIIコードで1を送信すると、リモートスタートする。
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
        uint8_t c = fgetc(bt); /* 受信 */
        switch(c)
        {
        case '1':
            bt_cmd = 1;
            break;
        default:
            break;
        }
        fputc(c, bt); /* エコーバック */
    }
}

//*****************************************************************************
// 関数名 : setLineTracePwm
// 引数 : brightness_ : ev3_color_sensor_get_reflect()の結果
// 　　 : forward_ : 前後進命令の変数
// 　　 : turn_ : 旋回命令の変数
// 返り値 : なし
// 概要 : 前後進値、旋回値を設定する
//*****************************************************************************
void setLineTracePwm(const int& brightness_, signed char& forward_, signed char& turn_){
    constexpr int LIGHT_WHITE = 40; /* 白色の光センサ値 */
    constexpr int LIGHT_BLACK = 0; /* 黒色の光センサ値 */
    
    forward_ = 60; /* 前進命令 */
    if(brightness_ >= (LIGHT_WHITE + LIGHT_BLACK)/2){
        turn_ =  30; /* 左旋回命令 */
    }else{
        turn_ = -30; /* 右旋回命令 */
    }
}
