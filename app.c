/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "app.h"
#include "app/framework/include/af.h"
#include "network-steering.h"
#include "zll-commissioning.h"
#include "find-and-bind-initiator.h"

#include "sl_simple_button_instances.h"
#include "sl_simple_button_btn3_config.h"
#include "sl_simple_button_btn1_config.h"
#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#define led_turn_on(led) sl_led_turn_on(led)
#define led_turn_off(led) sl_led_turn_off(led)
#define led_toggle(led) sl_led_toggle(led)

#define COMMISSIONING_STATUS_LED_0     (&sl_led_led0)
#define COMMISSIONING_STATUS_LED_1     (&sl_led_led1)
#define COMMISSIONING_STATUS_LED_2     (&sl_led_led2)
#define COMMISSIONING_STATUS_LED_3     (&sl_led_led3)

#else // !SL_CATALOG_LED0_PRESENT
#define led_turn_on(led)
#define led_turn_off(led)
#define led_toggle(led)
#endif // SL_CATALOG_LED0_PRESENT

#define LED_BLINK_PERIOD_MS          2000
#define TRANSITION_TIME_DS           20
#define FINDING_AND_BINDING_DELAY_MS 3000
#define BUTTON0                      0
#define BUTTON1                      1
#define BUTTON2                      2
#define BUTTON3                      3

#define SWITCH_ENDPOINT              1
#define SWITCH2_ENDPOINT             2
#define SWITCH3_ENDPOINT             3

#define TIME_FLASH_REMIND              500    //配网闪烁提醒

#define   KEY_OUT_TIME    3000                //按键认为是长按的判断时间
#define   LONG_TIMES      2 * KEY_OUT_TIME    //长按进入配网时间

#define   Battery_Time    5*60*1000           //有接收或发送任何消息， 更新时间为5分钟后再检测电量。


#define   Sigle_KEY          0
#define   Double_KEY         1
#define   Long_KEY           2
#define   Triple_KEY         3
#define   Clockwise_KEY      4
#define   AntiClockwise_KEY  5




static bool commissioning = false;


//static sl_zigbee_event_t commissioning_event;
//static sl_zigbee_event_t led_event;
static sl_zigbee_event_t finding_and_binding_event;
static sl_zigbee_event_t key0_manage_event;
static sl_zigbee_event_t key1_manage_event;
static sl_zigbee_event_t key2_manage_event;
static sl_zigbee_event_t key3_manage_event;
static sl_zigbee_event_t enter_join_net_event;
static sl_zigbee_event_t ResetKey_manage_event;
static sl_zigbee_event_t rejion_worknet_event;
static sl_zigbee_event_t POLL_Hearbeat_event;
static sl_zigbee_event_t Check_Battery_event;
static sl_zigbee_event_t knob_rotation_event;
static sl_zigbee_event_t Send_Hue_Delay_event;


//---------------
// Event handlers

void join_net_work(uint8_t join_v);
void report_power_config(void);

G_USER_CTRL_T tUserCtrl ={0,};
G_USER_CTRL_T *tpUserControl =&tUserCtrl;


G_USER_KEY_T tUserKey ={0,};
G_USER_KEY_T *tpUserKey =&tUserKey;


#define   User_Dat_ID   1   //存储对象标识

#define   Mark_Head     0x55
#define   Mark_End      0xaa


void nvm3_user_init(void)
{

//  nvm3_Handle_t handle;
  // Declare a nvm3_Init_t struct of name nvm3Data1 with initialization data. This is passed to nvm3_open() below.
 // NVM3_DEFINE_SECTION_INIT_DATA(nvm3Data1, &nvm3_halFlashHandle);

  G_USER_CTRL_T tUserInit ={0,};
  G_USER_CTRL_T *tpUserInit =&tUserInit;
  Ecode_t status;
//  uint32_t objectType;

  //status = nvm3_open(&handle, &nvm3Data1);  //打开 nvm3存储器
  status = nvm3_initDefault();
  sl_zigbee_app_debug_println("oPen status: 0x%x \n",status);
  if (status != ECODE_NVM3_OK) {
    // Handle error
      sl_zigbee_app_debug_println("oPen nvm3 error \n");
  }



#if  0
    size_t dataLen1;
    //读取对应句柄中的存储数量，如果没有对象写入初始值，
    // Get the number of valid keys already in NVM3
    dataLen1 = nvm3_countObjects(nvm3_defaultHandle);

    sl_zigbee_app_debug_println("object dataLen1 %d\n",dataLen1);
      //想要添加新的对象标识符时可以先查一下总共定义了多少个标识nvm3_countObjects(nvm3_defaultHandle);
         //然后使用下面的nvm3_enumObjects   跑遍所有的标识，将标识名打印出来，这样可以确保新添加的标识符不会与旧的重复
    nvm3_ObjectKey_t keyList[dataLen1];
    size_t numKeys = nvm3_enumObjects(nvm3_defaultHandle, keyList, dataLen1, NVM3_KEY_MIN, NVM3_KEY_MAX);
    for (size_t i = 0; i < numKeys; i++) {
        nvm3_ObjectKey_t key = keyList[i];
        // 打印对象标识
         sl_zigbee_app_debug_println("Object Key: %u\n", key);
    }
#endif
    // Skip if we have initial keys. If not, generate objects and store
    // persistently in NVM3 before proceeding.

  // nvm3_getObjectInfo(nvm3_defaultHandle, User_Dat_ID, &objectType, &dataLen1);
  // sl_zigbee_app_debug_println("objectType: %d dataLen1 %d\n",objectType,dataLen1);
   status = nvm3_readData(nvm3_defaultHandle, User_Dat_ID, tpUserInit, sizeof(G_USER_CTRL_T));
  // sl_zigbee_app_debug_println("INIT  MarkHead: %d MarkEnd %d\n",tpUserInit->MarkHead,tpUserInit->MarkEnd);
   if(status != 0){
      tpUserInit->MarkHead       = Mark_Head;
      tpUserInit->MarkEnd        = Mark_End;
      tpUserInit->Brightness     = 100;            //
      tpUserInit->Color_Temp     = 250;            //
      tpUserInit->Onoff_1        = 0;              //
      tpUserInit->Onoff_2        = 0;              //
      tpUserInit->Onoff_3        = 0;              //
      tpUserInit->Onoff_4        = 0;              //
      tpUserInit->Power_On_Times = 0;
      tpUserInit->Power_Per      = 100;
      tpUserInit->Power_Vol      = 30;
      tpUserInit->Control_Mode   = 0;
      memcpy(tpUserControl, tpUserInit, sizeof(G_USER_CTRL_T)); //写入应用
      status = nvm3_writeData(nvm3_defaultHandle, User_Dat_ID, &tUserInit, sizeof(G_USER_CTRL_T)); //写入flash
      sl_zigbee_app_debug_println("INIT write status: 0x%x \n",status);

   }else{
     if((tpUserInit->MarkHead != Mark_Head) || (tpUserInit->MarkEnd != Mark_End))
     {
         sl_zigbee_app_debug_println("chage  MarkHead: %d MarkEnd %d\n",tpUserInit->MarkHead,tpUserInit->MarkEnd);
         tpUserInit->MarkHead       = Mark_Head;
         tpUserInit->MarkEnd        = Mark_End;
         tpUserInit->Brightness     = 100;            //
         tpUserInit->Color_Temp     = 250;            //
         tpUserInit->Onoff_1        = 0;              //
         tpUserInit->Onoff_2        = 0;              //
         tpUserInit->Onoff_3        = 0;              //
         tpUserInit->Onoff_4        = 0;              //
         tpUserInit->Power_On_Times = 0;
         tpUserInit->Power_Per      = 100;
         tpUserInit->Power_Vol      = 30;
         tpUserInit->Control_Mode   = 0;
     }
     tpUserInit->Power_Per      = 100;      //每次通电都是默认为最大电量值，后续再检测更新值
     tpUserInit->Power_Vol      = 30;
     sl_zigbee_app_debug_println("tpUserInit->Power_On_Times: %d\n",tpUserInit->Power_On_Times);
     if(++tpUserInit->Power_On_Times >=3){
      //   tpUserInit->Power_On_Times =0;    //短时间连续上电3次，执行重置配网
      //   join_net_work(1);

         sl_zigbee_app_debug_println("join_net-work \n");
     }
     memcpy(tpUserControl, tpUserInit, sizeof(G_USER_CTRL_T)); //写入应用
     nvm3_writeData(nvm3_defaultHandle, User_Dat_ID, tpUserInit, sizeof(G_USER_CTRL_T)); //写入flash
   }
   tpUserControl->Power_Per      = 100;      //每次通电都是默认为最大电量值，后续再检测更新值
   tpUserControl->Power_Vol      = 30;
   emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID, (uint8_t*)&tpUserControl->Power_Vol, ZCL_INT8U_ATTRIBUTE_TYPE);
   emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID, (uint8_t*)&tpUserControl->Power_Per, ZCL_INT8U_ATTRIBUTE_TYPE);
   // 关闭 NVM3 控制器
 //  nvm3_deinitDefault();
 //  nvm3_close(nvm3_defaultHandle);
}



uint8_t  r_write_flag =0;
static uint8_t delay_write =0;
void set_write_flag(uint8_t flag)
{
  r_write_flag =flag;
  delay_write  =0;
  sl_zigbee_app_debug_println("set write flash\n");

}

void clean_flash(void)
{
  G_USER_CTRL_T tUserInit ={0,};
  G_USER_CTRL_T *tpUserInit =&tUserInit;
  tpUserInit->MarkHead       = Mark_Head;
  tpUserInit->MarkEnd        = Mark_End;
  tpUserInit->Brightness     = 100;            //
	tpUserInit->Color_Temp     = 250;            //
	tpUserInit->Onoff_1        = 0;              //
	tpUserInit->Onoff_2        = 0;              //
	tpUserInit->Onoff_3        = 0;              //
	tpUserInit->Power_On_Times = 0;
  tpUserInit->Power_Per      = 100;
  tpUserInit->Power_Vol      = 30;
  tpUserInit->Control_Mode   = tpUserControl->Control_Mode;
  memcpy(tpUserControl, tpUserInit, sizeof(G_USER_CTRL_T)); //写入应用
  nvm3_writeData(nvm3_defaultHandle, User_Dat_ID, &tUserInit, sizeof(G_USER_CTRL_T)); //写入flash
  sl_zigbee_app_debug_println("clean_flash :  \n");
}

void write_flash(void)
{
  if(++delay_write >=50){
      delay_write =0;     //250MS写入flash    主循环时间不稳定
    if(r_write_flag){
      r_write_flag =0;
      G_USER_CTRL_T tUserInit ={0,};
      G_USER_CTRL_T *tpUserInit =&tUserInit;
      nvm3_readData(nvm3_defaultHandle, User_Dat_ID, tpUserInit, sizeof(G_USER_CTRL_T));
    //  sl_zigbee_app_debug_println("read00 ->light: %d   color =%d\n",tpUserInit->Brightness,tpUserInit->Color_Temp);

      if((tpUserInit->MarkHead == Mark_Head) && (tpUserInit->MarkEnd == Mark_End)){
          EmberStatus status = nvm3_writeData(nvm3_defaultHandle, User_Dat_ID, tpUserControl, sizeof(G_USER_CTRL_T)); //写入flash
          sl_zigbee_app_debug_println("write status: 0x%x \n",status);
          nvm3_readData(nvm3_defaultHandle, User_Dat_ID, tpUserInit, sizeof(G_USER_CTRL_T));
          sl_zigbee_app_debug_println("read ->Power_On_Times: %d   Onoff =%d  Onoff2 =%d  Onoff3 =%d\n",tpUserInit->Power_On_Times,tpUserInit->Onoff_1,tpUserInit->Onoff_2,tpUserInit->Onoff_3);

      }
    }
  }
}




//static app_timer_t app_opened_timer;
static uint8_t  in_net_count =0;
EmberStatus r_cancel_network =0;  //定时器调用重复配网标志
void join_net_work(uint8_t join_v)
{
  EmberStatus status =0;
  status  = emberAfNetworkState();
  sl_zigbee_app_debug_println("ember work status: 0x%02X\n",status);
  switch (status) {
      case EMBER_NO_NETWORK:
          // 如果当前没有加入任何 Zigbee 网络，则启动网络加入流程。
          status = emberAfPluginNetworkSteeringStart();
          sl_zigbee_app_debug_println("EMBER_NO_NETWORK state: 0x%02X",status);
          break;
      case EMBER_JOINING_NETWORK:
          // 如果当前正在尝试加入 Zigbee 网络，则不进行任何操作。
          break;
      case EMBER_JOINED_NETWORK:
          // 如果当前已经加入了 Zigbee 网络，则不进行任何操作。
          if(join_v){
          //  status = emberLeaveNetwork();  //先离开网络
         //   status = emberAfPluginNetworkSteeringStart();
            sl_zigbee_app_debug_println("EMBER_JOINED_NETWORK state: 0x%02X",status);
          }else{
              r_cancel_network =0;
              emberAfSetShortPollIntervalMsCallback(100); //配网成功，设置100MS 的data request
          }
          break;
      case EMBER_JOINED_NETWORK_NO_PARENT:
          // 如果当前已经加入了 Zigbee 网络，但是没有找到合适的父节点，则重新启动网络加入流程。
          emberLeaveNetwork();  //先离开网络
          emberAfPluginNetworkSteeringStop();
        //  emberAfPluginNetworkSteeringStart();
          r_cancel_network =1;
          break;
      default:
          // 对于其他的状态值，可以进行相应的错误处理。
          break;
  }

  if(join_v){
      emberAfZllResetToFactoryNew();
      clean_flash();    //清除flash内容
      r_cancel_network =1;
      in_net_count =0;    //保证入网闪烁时间为30S
      sl_zigbee_event_set_delay_ms(&rejion_worknet_event,
                                         TIME_FLASH_REMIND);
  }
}







//定时上报开关状态用于心跳
void time_report_hearbeat(void)
{
//  static uint8_t rand_tims=30;

  uint32_t tickCount  = halCommonGetInt32uMillisecondTick();
  if(HeartbeatTime){  //有查询协议信息来就不执行定时上报，没有就执行随机定时上报
    if((tickCount - Save_HbTime) >=HeartbeatTime)
    {
        sl_zigbee_app_debug_println(" ===  tickCount %d  Save_HbTime =%d \n",tickCount,Save_HbTime);
        sl_zigbee_app_debug_println("hearbeat %d\n",HeartbeatTime);
        HeartbeatTime =0;
        report_hearbeat();
    }
  }
#if 0
  else{
      if(r_30second >=rand_tims){
           rand_tims = 20+ (rand()%20);
           sl_zigbee_app_debug_println("hearbeat %d\n",rand_tims);
           r_30second =0;
           report_hearbeat();
       }
  }
#endif
}

void app_init(void)
{
#if 0
  app_timer_start(&app_opened_timer,
      Time_Count,
      app_opened_timer_cbbbx,
      NULL,
      true);
    set_rand_source();
    GPIO_PinModeSet(gpioPortA,3,
                            gpioModePushPull,
                            0);
#endif
    nvm3_user_init();
   initIADC();  //初始化ADC
   //初始化触发查询电量
   sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
}


void app_process_action(void)
{

}











void report_agreement(void *value,uint16_t len)
{
    if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
        sl_zigbee_app_debug_println("no network return report_agreement:%d\r\n");
        return;
    }
    uint8_t  type=emberAfCurrentCommand()->type;


    //   sl_zigbee_app_debug_println("groupId :%d  option %x  destinationEndpoint %d   type =%x\r\n",emberAfCurrentCommand()->apsFrame->groupId,
     //                              emberAfCurrentCommand()->apsFrame->options,emberAfCurrentCommand()->apsFrame->destinationEndpoint,emberAfCurrentCommand()->type);




    if(type == EMBER_INCOMING_MULTICAST){ //群组消息不回复  report
        return;
    }

   uint16_t  idx =0,i;
   uint8_t  status =0;
   uint8_t  attribute_record3[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE] ={0};



    //报开关状态
    idx =0;
    attribute_record3[idx++] = LOW_BYTE(0x0068);
    attribute_record3[idx++] = HIGH_BYTE(0x0068);//私有协议定义属性
    attribute_record3[idx++] = ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
  //  attribute_record3[idx++] = len;
    sl_zigbee_app_debug_print(" len 0x%02X ",len);
    uint8_t *val_ptr = (uint8_t*)value;

     for (i = 0; i < len; i++) {
        attribute_record3[idx++] = val_ptr[i]; // 解引用指针取得数据
        sl_zigbee_app_debug_print(" 0x%02X ",val_ptr[i]);
     }



     //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer  ZCL_BASIC_CLUSTER_ID  ZCL_GENERIC_TUNNEL_CLUSTER_ID
     status = emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_BASIC_CLUSTER_ID,attribute_record3,idx);
    sl_zigbee_app_debug_println("ReportAttributes_no_response: 0x%02X\n",status);
    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(0x01, 0x01);

    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
   // sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    //status2 = emberAfSendCommandUnicastToBindings();

    sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat); //有发送更新心跳时间
    sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
}



void report_endpoint_onoff(uint8_t endpoint,uint8_t value)
{
  uint8_t  idx =0;
   uint8_t  status =0;
   uint8_t attribute_record3[5] ={0};

   if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
      sl_zigbee_app_debug_println("no network return report_endpoint_onoff:%d\r\n");
      return;
   }
#if 0
     emberAfGetCommandApsFrame()->sourceEndpoint = endpoint;
     if (value) {
         emberAfFillCommandOnOffClusterOn();
     } else{
         emberAfFillCommandOnOffClusterOff();
     }
 //    sl_zigbee_app_debug_println("%s: 0x%02X", "3 Send to bindings", status);
#endif
     uint8_t  type=emberAfCurrentCommand()->type;


   //   sl_zigbee_app_debug_println("groupId :%d  option %x  destinationEndpoint %d   type =%x\r\n",emberAfCurrentCommand()->apsFrame->groupId,
    //                              emberAfCurrentCommand()->apsFrame->options,emberAfCurrentCommand()->apsFrame->destinationEndpoint,emberAfCurrentCommand()->type);




     if(type == EMBER_INCOMING_MULTICAST){ //群组消息不回复  report
         return;
     }

    //报开关状态
    idx =0;
    attribute_record3[idx++] = LOW_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID);
    attribute_record3[idx++] = HIGH_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID);
    attribute_record3[idx++] = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
    attribute_record3[idx++] = value;


    //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer
    status = emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_ON_OFF_CLUSTER_ID,attribute_record3,idx);

    sl_zigbee_app_debug_println("emberAfFillCommandGlobalServerToClientReportAttributes: 0x%02X\n",status);
    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(endpoint, 0x01);

    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
    sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    //status2 = emberAfSendCommandUnicastToBindings();
}




void report_endpoint_onoff_resever_key(uint8_t endpoint,uint8_t value)
{
  uint8_t  idx =0;
   uint8_t  status =0;
   uint8_t attribute_record3[5] ={0};

   if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
      sl_zigbee_app_debug_println("no network return report_endpoint_onoff:%d\r\n");
      return;
   }
     uint8_t  type=emberAfCurrentCommand()->type;


   //   sl_zigbee_app_debug_println("groupId :%d  option %x  destinationEndpoint %d   type =%x\r\n",emberAfCurrentCommand()->apsFrame->groupId,
    //                              emberAfCurrentCommand()->apsFrame->options,emberAfCurrentCommand()->apsFrame->destinationEndpoint,emberAfCurrentCommand()->type);

     if(type == EMBER_INCOMING_MULTICAST){ //群组消息不回复  report
         return;
     }

    //报开关状态
    idx =0;
    attribute_record3[idx++] = value;
    // 0 =单击，1=双击，2=长按

    //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer

    status = emberAfFillCommandOnOffCluster(Private_KEY_ATTRIBUTE,attribute_record3,idx);
    sl_zigbee_app_debug_println("emberAfFillCommandOnOffCluster: 0x%02X\n",status);
    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(endpoint, 0x01);

    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);

    sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat); //有发送更新心跳时间
    sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
    //status2 = emberAfSendCommandUnicastToBindings();
}



//场景按键私有上报
void report_onoff_reserved(uint8_t value)
{
  uint8_t  idx =0;
   uint8_t  status =0;
   uint8_t attribute_record3[5] ={0};

   if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
      sl_zigbee_app_debug_println("no network return report_endpoint_onoff:%d\r\n");
      return;
   }
     uint8_t  type=emberAfCurrentCommand()->type;

     if(type == EMBER_INCOMING_MULTICAST){ //群组消息不回复  report
         return;
     }

    //报开关状态
    idx =0;
    if(value == Triple_KEY){  //三击中，发送切换模式， report attributes 8004
        attribute_record3[idx++] = LOW_BYTE(Private_Mode_ATTRIBUTE);
        attribute_record3[idx++] = HIGH_BYTE(Private_Mode_ATTRIBUTE);
        attribute_record3[idx++] = ZCL_ENUM8_ATTRIBUTE_TYPE;
        attribute_record3[idx++] = tpUserControl->Control_Mode;

        //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer
        emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_ON_OFF_CLUSTER_ID,attribute_record3,idx);
    }else{
      if(value == Clockwise_KEY){
          attribute_record3[idx++] = 0;
          emberAfFillCommandOnOffCluster(Private_ROTATE_ATTRIBUTE,attribute_record3,idx);
      }else if(value == AntiClockwise_KEY){
          attribute_record3[idx++] = 1;
          emberAfFillCommandOnOffCluster(Private_ROTATE_ATTRIBUTE,attribute_record3,idx);
      }else{
        attribute_record3[idx++] = value;
        // 0 =单击，1=双击，2=长按
        emberAfFillCommandOnOffCluster(Private_KEY_ATTRIBUTE,attribute_record3,idx);
      }
    }

    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(0x01, 0x01);
    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);

    sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat); //有发送更新心跳时间
    sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
    //status2 = emberAfSendCommandUnicastToBindings();

    //  emberAfSendCommandMulticast(0x1234);
}

#define  Level_UP          0x00
#define  Level_DOWN        0x01
#define  Saturation_UP     0x01
#define  Saturation_DOWN   0x03
#define  HUE_UP            0x01
#define  HUE_DOWN          0x03
#define  Temperature_UP    0x01
#define  Temperature_DOWN  0x03
#define  Temperature_MIN   2700      //以百分比来使用
#define  Temperature_MAX   6500



void Group_Send_Standard_Control(uint8_t cmd, uint8_t dir,uint8_t Data)
{
    uint8_t    StepMode =0;
    uint16_t   StepSize=0;
    uint16_t   TransitionTime=0;
    uint16_t   Get_Group =0;
    if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
       sl_zigbee_app_debug_println("no network return Group_Send_Standard_Control:%d\r\n");
       return;
    }

    switch(cmd){
     case OnOffToggle:
       emberAfFillCommandOnOffClusterToggle();
       break;
     case Level_Control_Step:
       if(dir){
           StepMode = Level_UP;
       }else{
           StepMode = Level_DOWN;
       }
       StepSize = Data*5;                 //20位定位器 步进值5  按百分比计算
       TransitionTime = 0xffff;//StepSize *10;     //转换时间按1MS的时间来变化
                                               //步进模式    步进值    变化时间
       emberAfFillCommandLevelControlClusterStep(StepMode,StepSize,TransitionTime,NULL,NULL);
      // emberAfFillCommandLevelControlClusterMove(StepMode,StepSize,NULL,NULL);
       break;
     case Color_Move_Saturation:  //饱和度每秒增加200个值
       emberAfFillCommandColorControlClusterMoveSaturation(Saturation_UP,200,NULL,NULL);
       break;
     case Color_Move_Hue:         //色度 每秒步进 15个值
       emberAfFillCommandColorControlClusterMoveHue(HUE_UP,15,NULL,NULL);
       break;
     case Color_Move_Stop:
       emberAfFillCommandColorControlClusterStopMoveStep(NULL,NULL);
       break;
     case Color_Temperature_Step:
       if(dir){
          StepMode = Temperature_UP;     //色温+ 变暖色
      }else{
          StepMode = Temperature_DOWN;         //色温- 变冷色
      }
      StepSize = Data*5 *38;                 //20位定位器 步进值5  按百分比计算   2700到6500 步进38
      TransitionTime = 1;//StepSize *10;     //转换时间按1MS的时间来变化
                                              //步进模式    步进值    变化时间
      emberAfFillCommandColorControlClusterStepColorTemperature(StepMode,StepSize,TransitionTime,Temperature_MIN,Temperature_MAX,NULL,NULL)
       break;
    }
    //设置收发端点
    emberAfSetCommandEndpoints(0x01, 0x01);

    //查找绑定的组ID
    for (uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
        EmberBindingTableEntry entry;
        if ((emberGetBinding(i, &entry) == EMBER_SUCCESS)
            && (entry.type == EMBER_MULTICAST_BINDING)) {
            Get_Group = (entry.identifier[1]<<8) | entry.identifier[0];
            break;
        }
      }
    //朝组ID，发送多播
    emberAfSendCommandMulticast(Get_Group);
    sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat); //有发送更新心跳时间
    sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
    sl_zigbee_app_debug_println("emberAfSendCommandUnicast Get_Group =%d ",Get_Group);
}




uint8_t g_endpoint =0;
uint8_t r_temp_onoff =0;
void emberAfPostAttributeChangeCallback(uint8_t endpoint,
                                        EmberAfClusterId clusterId,
                                        EmberAfAttributeId attributeId,
                                        uint8_t mask,
                                        uint16_t manufacturerCode,
                                        uint8_t type,
                                        uint8_t size,
                                        uint8_t* value)
{

  g_endpoint = endpoint;
//  sl_zigbee_app_debug_println("endpoint =%d clusterId 0x%02x attributeId 0x%02x:  mask 0x%02X  value =%d\n",endpoint,clusterId, attributeId, mask,*value);
  if (clusterId == ZCL_ON_OFF_CLUSTER_ID
      && attributeId == ZCL_ON_OFF_ATTRIBUTE_ID
      && mask == CLUSTER_MASK_SERVER) {
      if (emberAfReadServerAttribute(endpoint,
                                     ZCL_ON_OFF_CLUSTER_ID,
                                     ZCL_ON_OFF_ATTRIBUTE_ID,
                                     (uint8_t *)&r_temp_onoff,
                                     sizeof(r_temp_onoff))
          == EMBER_ZCL_STATUS_SUCCESS)
      {
          //回复状态值
          return;
          sl_zigbee_app_debug_println("recv attribute onoff =%d , endpoint:%d\r\n",r_temp_onoff, endpoint);
          if(g_endpoint == 0x01){
              if(r_temp_onoff !=tpUserControl->Onoff_1){
                  set_write_flag(1);
              }
              tpUserControl->Onoff_1 = r_temp_onoff ;
              report_endpoint_onoff(g_endpoint,tpUserControl->Onoff_1);
          }else  if(g_endpoint == 0x02){
              if(r_temp_onoff !=tpUserControl->Onoff_2){
                  set_write_flag(1);
              }
              tpUserControl->Onoff_2 = r_temp_onoff ;
              report_endpoint_onoff(g_endpoint,tpUserControl->Onoff_2);
          }else  if(g_endpoint == 0x03){
              if(r_temp_onoff !=tpUserControl->Onoff_3){
                  set_write_flag(1);
              }
                  tpUserControl->Onoff_3 = r_temp_onoff ;
                  set_write_flag(1);
              
              report_endpoint_onoff(g_endpoint,tpUserControl->Onoff_3);
          }

      }
  }

}



/*
static void commissioning_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    emberAfGetCommandApsFrame()->sourceEndpoint = SWITCH_ENDPOINT;
    if (lastButton == BUTTON0) {
      emberAfFillCommandOnOffClusterToggle();
    } else if (lastButton == BUTTON1) {
      uint8_t nextLevel = (uint8_t)(0xFF & emberGetPseudoRandomNumber());
      emberAfFillCommandLevelControlClusterMoveToLevel(nextLevel, TRANSITION_TIME_DS, 0, 0);
    }
    status = emberAfSendCommandUnicastToBindings();
    sl_zigbee_app_debug_println("%s: 0x%02X", "Send to bindings", status);
  } else {
    bool touchlink = (lastButton == BUTTON1);
    status = (touchlink
              ? emberAfZllInitiateTouchLink()
              : emberAfPluginNetworkSteeringStart());
    sl_zigbee_app_debug_println("%s network %s: 0x%02X",
                                (touchlink ? "Touchlink" : "Join"),
                                "start",
                                status);

 //   sl_zigbee_event_set_active(&led_event);

    commissioning = true;
  }
}

static void led_event_handler(sl_zigbee_event_t *event)
{
  if (commissioning) {
    if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
      led_toggle(COMMISSIONING_STATUS_LED);
      sl_zigbee_event_set_delay_ms(&led_event, LED_BLINK_PERIOD_MS << 1);
    } else {
      led_turn_on(COMMISSIONING_STATUS_LED);
    }
  } else if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    led_turn_on(COMMISSIONING_STATUS_LED);
  }
}
*/
static void finding_and_binding_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status = emberAfPluginFindAndBindInitiatorStart(SWITCH_ENDPOINT);
  sl_zigbee_app_debug_println("Find and bind initiator %s: 0x%02X", "start", status);

  /*
  status = emberAfPluginFindAndBindInitiatorStart(2);
  sl_zigbee_app_debug_println("2Find and bind initiator %s: 0x%02X", "start", status);

  status = emberAfPluginFindAndBindInitiatorStart(3);
  sl_zigbee_app_debug_println("3Find and bind initiator %s: 0x%02X", "start", status);
  */
}

//----------------------
// Implemented Callbacks



static void ResetKey_manage_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->key0_flag){
      tpUserKey->key0_flag =0;
      join_net_work(1);   //长按进入配网
  }
  sl_zigbee_app_debug_print("ResetKey_manage_event_handler\n" );
}



static void Send_Hue_Delay_event_handler(sl_zigbee_event_t *event)
{
  Group_Send_Standard_Control(Color_Move_Hue,0,0);
}



static void key0_manage_event_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->key0_flag){
#if 0   //取消单个按键长按进入配网
      if((halCommonGetInt32uMillisecondTick() - tpUserKey->key0_save_tick) >=LONG_TIMES){//长按6S配网
          join_net_work(1);
          tpUserKey->key0_flag =0;
      }else{//按键持续按下，继续计时长按
          sl_zigbee_event_set_delay_ms(&key0_manage_event,
                                     KEY_OUT_TIME);   //再次延时调用
      }
#endif
  }else{
    //  sl_zigbee_app_debug_println("tpUserKey->key1_click_num %d\n",tpUserKey->key1_click_num);
      if(tpUserKey->key0_click_num >=2){ //双击
          report_endpoint_onoff_resever_key(1,Double_KEY);
          sl_zigbee_app_debug_println("double key\n");
      }else if(tpUserKey->key0_click_num ==1){ //单击
          report_endpoint_onoff_resever_key(1,Sigle_KEY);
          sl_zigbee_app_debug_println("short key\n");
      }
  }
  if((r_cancel_network==0)){
    led_turn_off(COMMISSIONING_STATUS_LED_0); //只亮一次唤醒时间
  }
 // sl_zigbee_app_debug_print("key1_manage_event_event_handler  key1_click_num =%d\n",tpUserKey->key1_click_num );
  tpUserKey->key0_click_num =0;
}


static void key1_manage_event_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->key1_flag){
#if 0   //取消单个按键长按进入配网
      if((halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick) >=LONG_TIMES){//长按6S配网
          join_net_work(1);
          tpUserKey->key1_flag =0;
      }else{//按键持续按下，继续计时长按
          sl_zigbee_event_set_delay_ms(&key1_manage_event,
                                     KEY_OUT_TIME);   //再次延时调用
      }
#endif
  }else{
    //  sl_zigbee_app_debug_println("tpUserKey->key1_click_num %d\n",tpUserKey->key1_click_num);
      if(tpUserKey->key1_click_num >=2){ //双击
          report_endpoint_onoff_resever_key(2,Double_KEY);
          sl_zigbee_app_debug_println("double key\n");
      }else if(tpUserKey->key1_click_num ==1){ //单击
          report_endpoint_onoff_resever_key(2,Sigle_KEY);
          sl_zigbee_app_debug_println("short key\n");
      }
  }
  if((r_cancel_network==0)){
    led_turn_off(COMMISSIONING_STATUS_LED_1); //只亮一次唤醒时间
  }
 // sl_zigbee_app_debug_print("key1_manage_event_event_handler  key1_click_num =%d\n",tpUserKey->key1_click_num );
  tpUserKey->key1_click_num =0;
}



static void key2_manage_event_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->key2_flag){
#if 0   //取消单个按键长按进入配网
      if((halCommonGetInt32uMillisecondTick() - tpUserKey->key2_save_tick) >=LONG_TIMES){//长按6S配网
          join_net_work(1);
          tpUserKey->key2_flag =0;
      }else{//按键持续按下，继续计时长按
          sl_zigbee_event_set_delay_ms(&key2_manage_event,
                                     KEY_OUT_TIME);   //再次延时调用
      }
#endif
  }else{
    //  sl_zigbee_app_debug_println("tpUserKey->key1_click_num %d\n",tpUserKey->key1_click_num);
      if(tpUserKey->key2_click_num >=2){ //双击
          report_endpoint_onoff_resever_key(3,Double_KEY);
          sl_zigbee_app_debug_println("double key\n");
      }else if(tpUserKey->key2_click_num ==1){ //单击
          report_endpoint_onoff_resever_key(3,Sigle_KEY);
          sl_zigbee_app_debug_println("short key\n");
      }
  }
  if((r_cancel_network==0)){
    led_turn_off(COMMISSIONING_STATUS_LED_2); //只亮一次唤醒时间
  }
 // sl_zigbee_app_debug_print("key1_manage_event_event_handler  key1_click_num =%d\n",tpUserKey->key1_click_num );
  tpUserKey->key2_click_num =0;
}

static void key3_manage_event_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->key3_flag){
#if 0   //取消单个按键长按进入配网
      if((halCommonGetInt32uMillisecondTick() - tpUserKey->key3_save_tick) >=LONG_TIMES){//长按6S配网
          join_net_work(1);
          tpUserKey->key3_flag =0;
      }else{//按键持续按下，继续计时长按
          sl_zigbee_event_set_delay_ms(&key3_manage_event,
                                     KEY_OUT_TIME);   //再次延时调用
      }
#endif
  }else{
    //  sl_zigbee_app_debug_println("tpUserKey->key1_click_num %d\n",tpUserKey->key1_click_num);
      if(tpUserKey->key3_click_num >=2){ //双击
          report_endpoint_onoff_resever_key(4,Double_KEY);
          sl_zigbee_app_debug_println("double key\n");
      }else if(tpUserKey->key3_click_num ==1){ //单击
          report_endpoint_onoff_resever_key(4,Sigle_KEY);
          sl_zigbee_app_debug_println("short key\n");
      }
  }
  if((r_cancel_network==0)){
    led_turn_off(COMMISSIONING_STATUS_LED_3); //只亮一次唤醒时间
  }
 // sl_zigbee_app_debug_print("key1_manage_event_event_handler  key1_click_num =%d\n",tpUserKey->key1_click_num );
  tpUserKey->key3_click_num =0;
}


uint32_t join_net_tick =0;

static void enter_join_net_event_handler(sl_zigbee_event_t *event)
{

  if((halCommonGetInt32uMillisecondTick() - join_net_tick) >=5500){
      led_turn_off(COMMISSIONING_STATUS_LED_0);
      led_turn_off(COMMISSIONING_STATUS_LED_1);
      led_turn_off(COMMISSIONING_STATUS_LED_2);
      led_turn_off(COMMISSIONING_STATUS_LED_3);
      join_net_tick =0;
      join_net_work(1);  //进入配网
  }else{
      sl_zigbee_event_set_delay_ms(&enter_join_net_event,
                                       4000);   //再次延时调用
  }
  if(tpUserKey->key0_flag){   //无效化按键
      tpUserKey->key0_flag =0;
      tpUserKey->key0_click_num =0;
  }
  if(tpUserKey->key1_flag){   //无效化按键
      tpUserKey->key1_flag =0;
      tpUserKey->key1_click_num =0;
  }
  if(tpUserKey->key2_flag){   //无效化按键
      tpUserKey->key2_flag =0;
      tpUserKey->key2_click_num =0;
  }
  if(tpUserKey->key3_flag){   //无效化按键
      tpUserKey->key3_flag =0;
      tpUserKey->key3_click_num =0;
  }
}


void knob_rotation_event_handler(sl_zigbee_event_t *event)
{
  if(tpUserKey->anticlockwise_rotate_num > tpUserKey->clockwise_rotate_num){
      if(tpUserKey->anticlockwise_rotate_num){
          sl_zigbee_app_debug_println("anticlockwise_rotate_num %d ",tpUserKey->anticlockwise_rotate_num);
          if(tpUserControl->Control_Mode == Wireless_Switch){ //场景开关
              report_onoff_reserved(AntiClockwise_KEY);
          }else{                                              //遥控控制
              if((GPIO_PinInGet(SL_SIMPLE_BUTTON_BTN1_PORT,SL_SIMPLE_BUTTON_BTN1_PIN)==0)){ //按下调整色温
                  Group_Send_Standard_Control(Color_Temperature_Step,0,tpUserKey->anticlockwise_rotate_num);
              }else{                                                                        //非按下调整亮度
                  Group_Send_Standard_Control(Level_Control_Step,0,tpUserKey->anticlockwise_rotate_num);
              }

          }
      }
  }else{
      if(tpUserKey->clockwise_rotate_num){
          sl_zigbee_app_debug_println("clockwise_rotate_num %d ",tpUserKey->clockwise_rotate_num);
          if(tpUserControl->Control_Mode == Wireless_Switch){ //场景开关
              report_onoff_reserved(Clockwise_KEY);
          }else{
              if((GPIO_PinInGet(SL_SIMPLE_BUTTON_BTN1_PORT,SL_SIMPLE_BUTTON_BTN1_PIN)==0)){ //按下调整色温
                  Group_Send_Standard_Control(Color_Temperature_Step,1,tpUserKey->clockwise_rotate_num);
              }else{                                                                        //非按下调整亮度
                  Group_Send_Standard_Control(Level_Control_Step,1,tpUserKey->clockwise_rotate_num);
              }
          }
      }
  }
  if(!r_cancel_network){ led_turn_off(COMMISSIONING_STATUS_LED_0);}
  tpUserKey->rota_idr = tpUserKey->clockwise_rotate_num  = tpUserKey->anticlockwise_rotate_num =0;
}




static void rejion_worknet_event_handler(sl_zigbee_event_t *event)
{
  static uint8_t  sec_count =0;

  if(r_cancel_network == 1){
      led_toggle(COMMISSIONING_STATUS_LED_0);
      led_toggle(COMMISSIONING_STATUS_LED_1);
      led_toggle(COMMISSIONING_STATUS_LED_2);
      led_toggle(COMMISSIONING_STATUS_LED_3);
  }

  if(++sec_count >=6){
     sec_count =0;   //3S时间到
     if(r_cancel_network){
         join_net_work(0);
         if(++in_net_count >=15){    //
             in_net_count = 0;
             r_cancel_network =0;
         }
     }
     if(!r_cancel_network){
         //完成配网，关闭所有灯光
         led_turn_off(COMMISSIONING_STATUS_LED_0);
         led_turn_off(COMMISSIONING_STATUS_LED_1);
         led_turn_off(COMMISSIONING_STATUS_LED_2);
         led_turn_off(COMMISSIONING_STATUS_LED_3);
         report_power_config();    //心跳报告电量
     }
  }

  if(r_cancel_network){
      sl_zigbee_event_set_delay_ms(&rejion_worknet_event,
                                   TIME_FLASH_REMIND);
      sl_zigbee_app_debug_print("rejion_worknet_event_handler\n" );
  }
}



void report_power_config(void)
{
  uint8_t  idx =0;
    uint8_t  status =0;
    uint8_t attribute_record3[8] ={0};

    if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
       sl_zigbee_app_debug_println("no network return report_endpoint_onoff:%d\r\n");
       return;
    }

     //报开关状态
     idx =0;
     attribute_record3[idx++] = LOW_BYTE(ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
     attribute_record3[idx++] = HIGH_BYTE(ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
     attribute_record3[idx++] = ZCL_INT8U_ATTRIBUTE_TYPE;
     attribute_record3[idx++] = tpUserControl->Power_Per;  //0  -  100

     attribute_record3[idx++] = LOW_BYTE(ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID);
     attribute_record3[idx++] = HIGH_BYTE(ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID);
     attribute_record3[idx++] = ZCL_INT8U_ATTRIBUTE_TYPE;
     attribute_record3[idx++] = tpUserControl->Power_Vol;  //0  -  30

     sl_zigbee_app_debug_println("Power_Vol =%d  Power_Per =%d\n",tpUserControl->Power_Vol,tpUserControl->Power_Per);
  //   attribute_record3[idx++] = 0;

     //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer
   //  emberAfFillCommandGlobalClientToServerReserved(ZCL_POWER_CONFIG_CLUSTER_ID,attribute_record3,idx);
     status = emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_POWER_CONFIG_CLUSTER_ID,attribute_record3,idx);

  //   emberAfFillCommandOnOffClusterToggle();
     sl_zigbee_app_debug_println("report_power_config: 0x%02X\n",status);
     //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
     emberAfSetCommandEndpoints(0x01, 0x01);

     //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址

     status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
    // emberAfSendCommandMulticast(0x1234);
     sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat); //有发送更新心跳时间
     sl_zigbee_event_set_delay_ms(&Check_Battery_event, Battery_Time);  //有收发送更新查电量时间，查电量不会触发发送
     sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);

}



void Check_Battery_event_handler(sl_zigbee_event_t *event)
{
  static uint8_t Power_Per=0 ,Power_Vol=0;
  uint16_t temp_voltage =0;    //单位MV
  uint16_t temp_pre =0;

  temp_voltage = star_adc_tran();
  if(temp_voltage >=3000){      //电量100%
      tpUserControl->Power_Per = 100;
  }else{
      if(temp_voltage >2500){
          temp_pre = (temp_voltage -2500)/5;
      }else{
          temp_pre = 1;
      }
      if(tpUserControl->Power_Per > temp_pre){
          if((tpUserControl->Power_Per - temp_pre)>20){
              tpUserControl->Power_Per -= 20;       //下降超过20，最多认为是-20%
          }else{
              tpUserControl->Power_Per = temp_pre;
          }
      }
  }
 // tpUserControl->Power_Vol = temp_voltage/100;
  sl_zigbee_app_debug_println("cal Power_Vol =%d  Power_Per =%d\n",tpUserControl->Power_Vol,tpUserControl->Power_Per);
  if(Power_Per != tpUserControl->Power_Per){
      Power_Per = tpUserControl->Power_Per;
      emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID, (uint8_t*)&tpUserControl->Power_Per, ZCL_INT8U_ATTRIBUTE_TYPE);

  }
  if(Power_Vol != tpUserControl->Power_Vol){
      Power_Vol = tpUserControl->Power_Vol;
      emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID, (uint8_t*)&tpUserControl->Power_Vol, ZCL_INT8U_ATTRIBUTE_TYPE);
  }
}



void POLL_Hearbeat_event_handler(sl_zigbee_event_t *event)
{
    report_power_config();    //心跳报告电量
}


void emberAfMainInitCallback(void)
{
 // sl_zigbee_event_init(&commissioning_event, commissioning_event_handler);
 // sl_zigbee_event_init(&led_event, led_event_handler);
  sl_zigbee_event_init(&finding_and_binding_event, finding_and_binding_event_handler);
  sl_zigbee_event_init(&key0_manage_event, key0_manage_event_event_handler);
  sl_zigbee_event_init(&key1_manage_event, key1_manage_event_event_handler);
  sl_zigbee_event_init(&key2_manage_event, key2_manage_event_event_handler);
  sl_zigbee_event_init(&key3_manage_event, key3_manage_event_event_handler);
  sl_zigbee_event_init(&enter_join_net_event, enter_join_net_event_handler);

  sl_zigbee_event_init(&ResetKey_manage_event, ResetKey_manage_event_handler);
  sl_zigbee_event_init(&rejion_worknet_event, rejion_worknet_event_handler);
  sl_zigbee_event_init(&POLL_Hearbeat_event, POLL_Hearbeat_event_handler);
  sl_zigbee_event_init(&knob_rotation_event, knob_rotation_event_handler);
  sl_zigbee_event_init(&Send_Hue_Delay_event, Send_Hue_Delay_event_handler);
  sl_zigbee_event_init(&Check_Battery_event, Check_Battery_event_handler);
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action. The framework
 * will always process the stack status after the callback returns.
 */
void emberAfStackStatusCallback(EmberStatus status)
{
 // uint8_t rand_c =0;
  if (status == EMBER_NETWORK_DOWN) {
      sl_zigbee_app_debug_println("emberAfStackStatusCallback status =%d = EMBER_NETWORK_DOWN\n\n",status);
      EmberStatus status = emberAfNetworkState();
   //   led_turn_on(COMMISSIONING_STATUS_LED_0);    //断网 点亮
      if(status == EMBER_NO_NETWORK){
        sl_zigbee_app_debug_println("etner join_net_work(1)\n\n");
     //   join_net_work(1);   //掉线重新加网
      }

  //  led_turn_off(COMMISSIONING_STATUS_LED);
  } else if (status == EMBER_NETWORK_UP) {
      if(emberAfNetworkState() == EMBER_JOINED_NETWORK){
     //     led_turn_off(COMMISSIONING_STATUS_LED_0);    //通网 熄灭
        set_rand_source();
     //   rand_c = (rand()%500);
        tpUserControl->Power_Per      = 100;      //每次通电都是默认为最大电量值，后续再检测更新值
        tpUserControl->Power_Vol      = 30;
        report_power_config();    //心跳报告电量
      //  sl_zigbee_event_set_delay_ms(&POLL_Hearbeat_event,POLL_Hearbeat);
      }
  }
}

/** @brief Complete network steering.
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt.
 *
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID.
 *
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network.
 *
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete.
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  sl_zigbee_app_debug_println("%s network %s: 0x%02X", "Join", "complete", status);

  if (status != EMBER_SUCCESS) {
    commissioning = false;
  } else {
    sl_zigbee_event_set_delay_ms(&finding_and_binding_event,
                                 FINDING_AND_BINDING_DELAY_MS);
  }
}

/** @brief Touch Link Complete
 *
 * This function is called by the ZLL Commissioning Common plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the network
 * and target. Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target. Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target. Ver.: always
 */
void emberAfPluginZllCommissioningCommonTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                                  uint8_t deviceInformationRecordCount,
                                                                  const EmberZllDeviceInfoRecord *deviceInformationRecordList)
{
  sl_zigbee_app_debug_println("%s network %s: 0x%02X",
                              "Touchlink",
                              "complete",
                              EMBER_SUCCESS);

  sl_zigbee_event_set_delay_ms(&finding_and_binding_event,
                               FINDING_AND_BINDING_DELAY_MS);
}

/** @brief Touch Link Failed
 *
 * This function is called by the ZLL Commissioning Client plugin if touch linking
 * fails.
 *
 * @param status The reason the touch link failed. Ver.: always
 */
void emberAfPluginZllCommissioningClientTouchLinkFailedCallback(EmberAfZllCommissioningStatus status)
{
  sl_zigbee_app_debug_println("%s network %s: 0x%02X",
                              "Touchlink",
                              "complete",
                              EMBER_ERR_FATAL);

  commissioning = false;
}

/** @brief Find and Bind Complete
 *
 * This callback is fired by the initiator when the Find and Bind process is
 * complete.
 *
 * @param status Status code describing the completion of the find and bind
 * process Ver.: always
 */
void emberAfPluginFindAndBindInitiatorCompleteCallback(EmberStatus status)
{
  sl_zigbee_app_debug_println("Find and bind initiator %s: 0x%02X", "complete", status);

  commissioning = false;
}

#ifndef SL_CATALOG_ZIGBEE_EZSP_PRESENT
/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}
#endif //SL_CATALOG_ZIGBEE_EZSP_PRESENT

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT) && (SL_ZIGBEE_APP_FRAMEWORK_USE_BUTTON_TO_STAY_AWAKE == 0)
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

/***************************************************************************//**
 * A callback called in interrupt context whenever a button changes its state.
 *
 * @remark Can be implemented by the application if required. This function
 * can contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to appropriate functionality.
 *
 * @note The button state should not be updated in this function, it is updated
 * by specific button driver prior to arriving here
 *
   @param[out] handle             Pointer to button instance
 ******************************************************************************/


void sl_button_on_change(const sl_button_t *handle)
{
  if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON0) == handle) {       //switch  1
      if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
          if(tpUserKey->key0_flag){
             tpUserKey->key0_flag =0;
             tpUserKey->key0_click_num ++;
             sl_zigbee_app_debug_println("relax key0num =%d",tpUserKey->key0_click_num);
           //  sl_zigbee_app_debug_println("diff key tick %d \n",halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick);
             if((halCommonGetInt32uMillisecondTick() - tpUserKey->key0_save_tick) >=KEY_OUT_TIME){  //认为是长按
                 tpUserKey->key0_click_num =0;
                 report_endpoint_onoff_resever_key(1,Long_KEY);
                 sl_zigbee_event_set_inactive(&key0_manage_event);  //长按，取消延时事件，避免无效唤醒
             }else{  //非长按， 松开后立即快速判断一下是否是单双击
               sl_zigbee_event_set_delay_ms(&key0_manage_event,250);
             }

          }
          if(!r_cancel_network) {led_turn_off(COMMISSIONING_STATUS_LED_0);}
          if(join_net_tick){
            join_net_tick = 0;
            sl_zigbee_event_set_inactive(&enter_join_net_event);
          }
          sl_zigbee_app_debug_println("tpUserKey->key0_save_tick %d \n",tpUserKey->key0_save_tick);
      }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {

          if(!tpUserKey->key0_flag){
            tpUserKey->key0_flag =1;
            if(!r_cancel_network){ led_turn_on(COMMISSIONING_STATUS_LED_0);}
            if(tpUserKey->key2_flag || tpUserKey->key1_flag || tpUserKey->key3_flag){
               if(tpUserKey->key2_flag)
                 sl_zigbee_event_set_inactive(&key2_manage_event);
               if(tpUserKey->key1_flag)
                 sl_zigbee_event_set_inactive(&key1_manage_event);
               if(tpUserKey->key3_flag)
                 sl_zigbee_event_set_inactive(&key3_manage_event);
               if(join_net_tick == 0){
                   join_net_tick = halCommonGetInt32uMillisecondTick();
                   sl_zigbee_event_set_delay_ms(&enter_join_net_event,2000);
               }

            }else{
              tpUserKey->key0_save_tick = halCommonGetInt32uMillisecondTick();
              sl_zigbee_event_set_delay_ms(&key0_manage_event, KEY_OUT_TIME);  //延迟3S调用按键判断
              emberAfSetShortPollIntervalQsCallback(2);  //设置500ms  的data request
              sl_zigbee_app_debug_println("tpUserKey->key0_save_tick %d \n",tpUserKey->key0_save_tick);
            }
          }

          sl_zigbee_app_debug_println("tpUserKey->key0_save_tick %d \n",tpUserKey->key0_save_tick);
      }
    } else if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON1) == handle) {       //switch  1
    if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
       if(tpUserKey->key1_flag){
          tpUserKey->key1_flag =0;
          tpUserKey->key1_click_num ++;
          sl_zigbee_app_debug_println("relax key1num =%d",tpUserKey->key1_click_num);
        //  sl_zigbee_app_debug_println("diff key tick %d \n",halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick);
          if((halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick) >=KEY_OUT_TIME){  //认为是长按
              tpUserKey->key1_click_num =0;
              report_endpoint_onoff_resever_key(2,Long_KEY);
              sl_zigbee_event_set_inactive(&key1_manage_event);  //长按，取消延时事件，避免无效唤醒
          }else{  //非长按， 松开后立即快速判断一下是否是单双击
            sl_zigbee_event_set_delay_ms(&key1_manage_event,250);
          }
       }
       if(!r_cancel_network) {led_turn_off(COMMISSIONING_STATUS_LED_1);}
       if(join_net_tick){
         join_net_tick = 0;
         sl_zigbee_event_set_inactive(&enter_join_net_event);
       }

    }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
#if(SWITCH_NUM>=1)
        if(!tpUserKey->key1_flag){
          tpUserKey->key1_flag =1;
          if(!r_cancel_network){ led_turn_on(COMMISSIONING_STATUS_LED_1);}
          if(tpUserKey->key0_flag || tpUserKey->key2_flag || tpUserKey->key3_flag){
             if(tpUserKey->key0_flag)
               sl_zigbee_event_set_inactive(&key0_manage_event);
             if(tpUserKey->key2_flag)
               sl_zigbee_event_set_inactive(&key2_manage_event);
             if(tpUserKey->key3_flag)
               sl_zigbee_event_set_inactive(&key3_manage_event);
             if(join_net_tick == 0){
                 join_net_tick = halCommonGetInt32uMillisecondTick();
                 sl_zigbee_event_set_delay_ms(&enter_join_net_event,2000);
             }

          }else{
            tpUserKey->key1_save_tick = halCommonGetInt32uMillisecondTick();
            sl_zigbee_event_set_delay_ms(&key1_manage_event, KEY_OUT_TIME);  //延迟3S调用按键判断
            emberAfSetShortPollIntervalQsCallback(2);  //设置500ms  的data request
            sl_zigbee_app_debug_println("tpUserKey->key1_save_tick %d \n",tpUserKey->key1_save_tick);
          }
        }
#endif
    }
  } else if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON2) == handle) {  //switch 2
    if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
        if(tpUserKey->key2_flag){
          tpUserKey->key2_flag =0;

          tpUserKey->key2_click_num ++;
          sl_zigbee_app_debug_println("relax key2num =%d",tpUserKey->key2_click_num);
        //  sl_zigbee_app_debug_println("diff key tick %d \n",halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick);
          if((halCommonGetInt32uMillisecondTick() - tpUserKey->key2_save_tick) >=KEY_OUT_TIME){  //认为是长按
              tpUserKey->key2_click_num =0;
              report_endpoint_onoff_resever_key(3,Long_KEY);
              sl_zigbee_event_set_inactive(&key2_manage_event);  //长按，取消延时事件，避免无效唤醒
          }else{  //非长按， 松开后立即快速判断一下是否是单双击
            sl_zigbee_event_set_delay_ms(&key2_manage_event,250);
          }

        }
        if(!r_cancel_network) {led_turn_off(COMMISSIONING_STATUS_LED_2);}
        if(join_net_tick){
          join_net_tick = 0;
          sl_zigbee_event_set_inactive(&enter_join_net_event);
        }
    }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
#if(SWITCH_NUM>=2)
        if(tpUserKey->key2_flag ==0){
          tpUserKey->key2_flag =1;
          if(!r_cancel_network){ led_turn_on(COMMISSIONING_STATUS_LED_2);}

          if(tpUserKey->key0_flag || tpUserKey->key1_flag || tpUserKey->key3_flag){
             if(tpUserKey->key0_flag)
               sl_zigbee_event_set_inactive(&key0_manage_event);
             if(tpUserKey->key1_flag)
               sl_zigbee_event_set_inactive(&key1_manage_event);
             if(tpUserKey->key3_flag)
               sl_zigbee_event_set_inactive(&key3_manage_event);
             if(join_net_tick == 0){
                 join_net_tick = halCommonGetInt32uMillisecondTick();
                 sl_zigbee_event_set_delay_ms(&enter_join_net_event,2000);
             }

          }else{

            tpUserKey->key2_save_tick = halCommonGetInt32uMillisecondTick();
            sl_zigbee_event_set_delay_ms(&key2_manage_event, KEY_OUT_TIME);  //延迟3S调用按键判断
            emberAfSetShortPollIntervalQsCallback(2);  //设置500ms  的data request
            sl_zigbee_app_debug_println("tpUserKey->key2_save_tick %d \n",tpUserKey->key2_save_tick);
          }
        }
#endif
    }
  }else if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON3) == handle) {
      if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
          if(tpUserKey->key3_flag){
            tpUserKey->key3_flag =0;
            tpUserKey->key3_click_num ++;
            sl_zigbee_app_debug_println("relax key2num =%d",tpUserKey->key3_click_num);
          //  sl_zigbee_app_debug_println("diff key tick %d \n",halCommonGetInt32uMillisecondTick() - tpUserKey->key1_save_tick);
            if((halCommonGetInt32uMillisecondTick() - tpUserKey->key3_save_tick) >=KEY_OUT_TIME){  //认为是长按
                tpUserKey->key3_click_num =0;
                report_endpoint_onoff_resever_key(4,Long_KEY);
                sl_zigbee_event_set_inactive(&key3_manage_event);  //长按，取消延时事件，避免无效唤醒
            }else{  //非长按， 松开后立即快速判断一下是否是单双击
              sl_zigbee_event_set_delay_ms(&key3_manage_event,250);
            }
          }
          if(!r_cancel_network) {led_turn_off(COMMISSIONING_STATUS_LED_3);}
          if(join_net_tick){
            join_net_tick = 0;
            sl_zigbee_event_set_inactive(&enter_join_net_event);
          }
      }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
#if(SWITCH_NUM>=3)
          if(tpUserKey->key3_flag ==0){
            tpUserKey->key3_flag =1;
           if(!r_cancel_network){ led_turn_on(COMMISSIONING_STATUS_LED_3);}

           if(tpUserKey->key0_flag || tpUserKey->key1_flag || tpUserKey->key2_flag){
               if(tpUserKey->key0_flag)
                 sl_zigbee_event_set_inactive(&key0_manage_event);
               if(tpUserKey->key1_flag)
                 sl_zigbee_event_set_inactive(&key1_manage_event);
               if(tpUserKey->key2_flag)
                 sl_zigbee_event_set_inactive(&key2_manage_event);
               if(join_net_tick == 0){
                   join_net_tick = halCommonGetInt32uMillisecondTick();
                   sl_zigbee_event_set_delay_ms(&enter_join_net_event,2000);
               }

           }else{
              tpUserKey->key3_save_tick = halCommonGetInt32uMillisecondTick();
              sl_zigbee_event_set_delay_ms(&key3_manage_event, KEY_OUT_TIME);  //延迟3S调用按键判断 采用2个按键长按配网的形式，这里的目的就只剩下做一个指示灯提示用户长按时间够了
              emberAfSetShortPollIntervalQsCallback(2);  //设置500ms  的data request
              sl_zigbee_app_debug_println("tpUserKey->key3_save_tick %d \n",tpUserKey->key3_save_tick);
           }

          }
#endif
      }
    }
}
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT && SL_ZIGBEE_APP_FRAMEWORK_USE_BUTTON_TO_STAY_AWAKE == 0

//Internal testing stuff
#if defined(EMBER_TEST)
void emberAfHalButtonIsrCallback(uint8_t button,
                                 uint8_t state)
{
  if (state == BUTTON_RELEASED) {
    lastButton = button;
    sl_zigbee_event_set_active(&commissioning_event);
  }
}
#endif // EMBER_TEST
