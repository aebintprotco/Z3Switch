/*
 * app.h
 *
 *  Created on: 2023年6月27日   jjjj
 *      Author: YWJ
 */

#ifndef APP_H_
#define APP_H_


#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "sl_simple_rgb_pwm_led.h"

//---------------
// Event handlers
#include "nvm3.h"
#include "nvm3_hal_flash.h"

#include "app_timer.h"


#include "af.h"
void initIADC (void);
uint16_t star_adc_tran(void);

#define SWITCH_NUM                   3
#define SCENE_NUM                    0

#define POLL_Hearbeat      HEARBEAT_TIME *1000

#define Private_Mode_ATTRIBUTE           0x8004

#define Private_KEY_ATTRIBUTE              0xFD
#define Private_ROTATE_ATTRIBUTE           0xFC


#define Remote_Control           0
#define Wireless_Switch          1

void poll_hearbeat_event_handler(sl_zigbee_event_t *event);

typedef struct {
  uint8_t MarkHead;
  uint8_t Power_On_Times;//上电次数
  uint8_t Onoff_1;    //开关
  uint8_t Onoff_2;    //开关
  uint8_t Onoff_3;    //开关
  uint8_t Onoff_4;    //开关
  uint8_t backlight_16;    //背光开关
  uint8_t Brightness;  //亮度
  uint8_t resever;     //保留
  uint8_t Control_Mode; //控制模式，0=remote_control,  1=wireless_switch 场景开关
  uint8_t Power_Per;  //剩余电量
  uint8_t Power_Vol;  //电压
  uint16_t Color_Temp;  //色温

  uint8_t MarkEnd;
} G_USER_CTRL_T;
extern G_USER_CTRL_T *tpUserControl;



typedef enum {
    OnOffToggle=0,
    Level_Control_Step,
    Color_Move_Saturation,
    Color_Move_Hue,
    Color_Move_Stop,
    Color_Temperature_Step,
} STANDARD_SEND_E;



typedef struct {
   bool  key0_flag;
   bool  key1_flag;
   bool  key2_flag;
   bool  key3_flag;
   uint8_t  key0_click_num;
   uint8_t  key1_click_num;
   uint8_t  key2_click_num;
   uint8_t  key3_click_num;
   uint8_t rota_idr;
   uint8_t clockwise_rotate_num;
   uint8_t anticlockwise_rotate_num;
   uint32_t  key0_save_tick;
   uint32_t  key1_save_tick;
   uint32_t  key2_save_tick;
   uint32_t  key3_save_tick;

} G_USER_KEY_T;
extern G_USER_KEY_T *tpUserKey;


extern uint16_t HeartbeatTime;
extern uint32_t Save_HbTime;
#define  Time_Count    10     //定时器设置10ms
void report_hearbeat(void);
void app_init(void);
void app_process_action(void);

void report_agreement(void *value,uint16_t len);
void set_write_flag(uint8_t flag);
void set_rand_source(void);

#endif /* APP_H_ */
