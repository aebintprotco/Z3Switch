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

#include "app/framework/include/af.h"
#include "network-steering.h"
#include "zll-commissioning.h"
#include "find-and-bind-initiator.h"

#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#define led_turn_on(led) sl_led_turn_on(led)
#define led_turn_off(led) sl_led_turn_off(led)
#define led_toggle(led) sl_led_toggle(led)

#define COMMISSIONING_STATUS_LED_0     (&sl_led_led0)
#define COMMISSIONING_STATUS_LED_1     (&sl_led_led1)
#define COMMISSIONING_STATUS_LED_2     (&sl_led_led2)

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
#define SWITCH_ENDPOINT              1

static bool commissioning = false;

void report_agreement(void *value,uint8_t len);

void report_hearbeat(void);
void filing_header(uint8_t cmd,uint8_t num);
//static sl_zigbee_event_t commissioning_event;
//static sl_zigbee_event_t led_event;
static sl_zigbee_event_t finding_and_binding_event;

//---------------
// Event handlers



#include "app_timer.h"
#include "nvm3.h"
#include "nvm3_hal_flash.h"

void join_net_work(uint8_t join_v);
typedef struct {
  uint8_t MarkHead;
  uint8_t MarkEnd;
  uint8_t Power_On_Times;//上电次数
  uint8_t Onoff;    //开关
  uint8_t Onoff_2;    //开关
  uint8_t Onoff_3;    //开关
  uint8_t Brightness;  //亮度
  uint8_t resever;     //保留
  uint16_t Color_Temp;  //色温

} G_USER_CTRL_T;
G_USER_CTRL_T tUserCtrl ={0,};
G_USER_CTRL_T *tpUserControl =&tUserCtrl;

void report_all_data(void);


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
      tpUserInit->Onoff          = 0;              //
      tpUserInit->Onoff_2        = 0;              //
      tpUserInit->Onoff_3        = 0;              //
      tpUserInit->Power_On_Times = 0;
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
         tpUserInit->Onoff          = 0;              //
         tpUserInit->Onoff_2        = 0;              //
         tpUserInit->Onoff_3        = 0;              //
         tpUserInit->Power_On_Times = 0;
     }
     sl_zigbee_app_debug_println("tpUserInit->Power_On_Times: %d\n",tpUserInit->Power_On_Times);
     if(++tpUserInit->Power_On_Times >=3){
      //   tpUserInit->Power_On_Times =0;    //短时间连续上电3次，执行重置配网
         join_net_work(1);

         sl_zigbee_app_debug_println("join_net-work \n");
     }
     memcpy(tpUserControl, tpUserInit, sizeof(G_USER_CTRL_T)); //写入应用
     nvm3_writeData(nvm3_defaultHandle, User_Dat_ID, tpUserInit, sizeof(G_USER_CTRL_T)); //写入flash
   }
   // 关闭 NVM3 控制器
 //  nvm3_deinitDefault();
 //  nvm3_close(nvm3_defaultHandle);
}


uint8_t  r_write_flag =0;
void set_write_flag(uint8_t flag)
{
  r_write_flag =flag;
  sl_zigbee_app_debug_println("set write flash\n");

}


void write_flash(void)
{
  static uint8_t delay_write =0;
  if(++delay_write >=3){
      delay_write =0;     //250MS写入flash    主循环时间不稳定

   //   led_toggle(&sl_led_led1);
   //   sl_zigbee_app_debug_println("start write flash\n");
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
          sl_zigbee_app_debug_println("read ->Power_On_Times: %d   Onoff =%d  Onoff2 =%d  Onoff3 =%d\n",tpUserInit->Power_On_Times,tpUserInit->Onoff,tpUserInit->Onoff_2,tpUserInit->Onoff_3);

      }
    }
  }
}




static app_timer_t app_opened_timer;
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
          if(join_v){
              r_cancel_network =1;
              in_net_count =0;    //保证入网闪烁时间为30S
          }
          break;
      case EMBER_JOINING_NETWORK:
          // 如果当前正在尝试加入 Zigbee 网络，则不进行任何操作。
          break;
      case EMBER_JOINED_NETWORK:
          // 如果当前已经加入了 Zigbee 网络，则不进行任何操作。
          if(join_v){
            emberAfZllResetToFactoryNew();
           // status = emberLeaveNetwork();  //先离开网络
         //   status = emberAfPluginNetworkSteeringStart();
            sl_zigbee_app_debug_println("EMBER_JOINED_NETWORK: 0x%02X",status);
            r_cancel_network =1;
          }else{
              r_cancel_network =0;
              report_all_data();
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

}

static bool  key1_flag =0;
static bool  key2_flag =0;
static bool  key3_flag =0;

void  key_enter_net(void)
{
  static uint8_t  key1_count =0;
  static uint8_t  key2_count =0;
  static uint8_t  key3_count =0;

  if(key1_flag){
      if(++key1_count >=12){  //长按3S配网
          key1_count =0;
          join_net_work(1);
          key1_flag =0;
      }

  }else{
      key1_count =0;
  }
  //key 2
  if(key2_flag){
      if(++key2_count >=12){  //长按3S配网
          key2_count =0;
          join_net_work(1);
          key2_flag =0;
      }

  }else{
      key2_count =0;
  }
  //key 3
  if(key3_flag){
      if(++key3_count >=12){  //长按3S配网
          key3_count =0;
          join_net_work(1);
          key3_flag =0;
      }

  }else{
      key3_count =0;
  }

}


static uint8_t  r_30second  =0;
void app_opened_timer_cbbbx(app_timer_t *timer, void *data)
{
  static bool init_power_flag =0;
  static uint8_t  sec_count =0;
  (void)data;
  (void)timer;
 //  EmberStatus status  = emberAfNetworkState();
 //  sl_zigbee_app_debug_println("ember work status: 0x%02X\n",status);

  if(++sec_count >=4){
     sec_count =0;   //1S时间到
     ++r_30second;
     if(r_cancel_network){
         join_net_work(0);
         if(++in_net_count >=30){    //300S超时。*1S
             in_net_count = 0;
             r_cancel_network =0;
         }
     }

     if(!init_power_flag){
         if(++in_net_count >=3){  //通电3S后清除上电数据。
             in_net_count =0;
             init_power_flag =1;
             tpUserControl->Power_On_Times = 0;
             set_write_flag(1);  //写入flash
             sl_zigbee_app_debug_println("3S  clean\n");
         }
     }
  }
  key_enter_net();
  if(r_cancel_network == 1){
      led_toggle(COMMISSIONING_STATUS_LED_0);
      led_turn_off(COMMISSIONING_STATUS_LED_1);
      led_turn_off(COMMISSIONING_STATUS_LED_2);

  }

}

void app_init(void)
{
  app_timer_start(&app_opened_timer,
      250,
      app_opened_timer_cbbbx,
      NULL,
      true);
  nvm3_user_init();
}

bool report_state =0;
void app_process_action(void)
{
  static uint32_t save_tick =0;


  uint32_t tickCount  = halCommonGetInt32uMillisecondTick();
  if(tickCount - save_tick >=10){ //10msִ
      save_tick = tickCount;

      write_flash();
      if(report_state){
          report_state =0;
          report_all_data();

      }
      if(r_30second>=30){
          r_30second =0;
          report_hearbeat();
      }
      if(r_cancel_network == 0){
         if(tpUserControl->Onoff){
             led_turn_on(COMMISSIONING_STATUS_LED_0);
         }else{
             led_turn_off(COMMISSIONING_STATUS_LED_0);
         }
         if(tpUserControl->Onoff_2){
             led_turn_on(COMMISSIONING_STATUS_LED_1);
         }else{
             led_turn_off(COMMISSIONING_STATUS_LED_1);
         }
         if(tpUserControl->Onoff_3){
             led_turn_on(COMMISSIONING_STATUS_LED_2);
         }else{
             led_turn_off(COMMISSIONING_STATUS_LED_2);
         }
      }
  }

 // sl_zigbee_app_debug_println("tickCount %d\n",tickCount);


}





void report_hearbeat(void)
{
  uint8_t  data[] ={0xff,0x99,0x01,0x01,0x00};
  report_agreement(data,5);
}
uint8_t filing_data[75]={0};
uint8_t filing_idx =0;

//填充数据包头
void filing_header(uint8_t cmd,uint8_t num)
{
  filing_idx =0;
  filing_data[filing_idx++] = 0xff;
  filing_data[filing_idx++] = cmd;
  filing_data[filing_idx++] = 0x01; // 总包数
  filing_data[filing_idx++] = 0x01; // 包序号
  filing_data[filing_idx++] = num;  //DP数量
}
//填充数据内容
void filing_device_data(uint16_t dp,uint8_t len, void *val)
{
  uint8_t  i;
  filing_data[filing_idx++] = dp;
  filing_data[filing_idx++] = len;
  // 将 void* 转换为 uint8_t* 指针
  uint8_t *val_ptr = (uint8_t*)val;

  for (i = 0; i < len; i++) {
    filing_data[filing_idx++] = val_ptr[i]; // 解引用指针取得数据

    sl_zigbee_app_debug_println(" rport val_ptr[i]: 0x%02X\n",val_ptr[i]);
  }
}



void report_agreement(void *value,uint8_t len)
{
    if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
        sl_zigbee_app_debug_println("no network return report_hearbeat:%d\r\n");
        return;
    }
   uint8_t  idx =0,i;
   uint8_t  status =0;
   uint8_t  attribute_record3[79] ={0};



    //报开关状态
    idx =0;
    attribute_record3[idx++] = LOW_BYTE(ZCL_PROTOCOL_ADDRESS_ATTRIBUTE_ID);
    attribute_record3[idx++] = HIGH_BYTE(ZCL_PROTOCOL_ADDRESS_ATTRIBUTE_ID);
    attribute_record3[idx++] = ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
    attribute_record3[idx++] = len;
    uint8_t *val_ptr = (uint8_t*)value;

     for (i = 0; i < len; i++) {
        attribute_record3[idx++] = val_ptr[i]; // 解引用指针取得数据

     }



    //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer
    status = emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_GENERIC_TUNNEL_CLUSTER_ID,attribute_record3,idx);
    sl_zigbee_app_debug_println("ReportAttributes_no_response: 0x%02X\n",status);
    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(0x01, 0x01);

    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
    sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    //status2 = emberAfSendCommandUnicastToBindings();
}




void report_endpoint(uint8_t endpoint,uint8_t value)
{
  uint8_t  idx =0;
   uint8_t  status =0;
   uint8_t attribute_record3[5] ={0};

   if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
      sl_zigbee_app_debug_println("no network return report_endpoint:%d\r\n");
      return;
   }
 /*    emberAfGetCommandApsFrame()->sourceEndpoint = endpoint;
     if (value) {
         emberAfFillCommandOnOffClusterOn();
     } else{
         emberAfFillCommandOnOffClusterOff();
     }
     sl_zigbee_app_debug_println("%s: 0x%02X", "3 Send to bindings", status);
*/


    //报开关状态
    idx =0;
    attribute_record3[idx++] = LOW_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID);
    attribute_record3[idx++] = HIGH_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID);
    attribute_record3[idx++] = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
    attribute_record3[idx++] = value;


    //将属性报告填充进入缓冲区   CLUSTER值   数据内容  数据长度      //Fill a ZCL global report attributes command buffer
    status = emberAfFillCommandGlobalServerToClientReportAttributes_no_response(ZCL_ON_OFF_CLUSTER_ID,attribute_record3,idx);
    sl_zigbee_app_debug_println("ReportAttributes_no_response: 0x%02X\n",status);
    //写入 sourceEndpoint  和 destinationEndpoint    //Specify endpoints for command sending
    emberAfSetCommandEndpoints(endpoint, 0x01);

    //调用发送数据接口将缓存好的数据发出           发生选项  和  单播目标地址
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
    sl_zigbee_app_debug_println("emberAfSendCommandUnicast: 0x%02X\n",status);
    //status2 = emberAfSendCommandUnicastToBindings();
}

#define SWITCH_NUM 3
void report_all_data(void)
{

  filing_header(0x06,(SWITCH_NUM));
#if(SWITCH_NUM >=1)
  sl_zigbee_app_debug_println(" rport tpUserControl->Onoff_1: 0x%02X\n",tpUserControl->Onoff);
  filing_device_data(1,1,&tpUserControl->Onoff);
  //report_endpoint_onoff(1,tpUserControl->Onoff_1);
#endif
#if(SWITCH_NUM >=2)
  filing_device_data(2,1,&tpUserControl->Onoff_2);
  //report_endpoint_onoff(2,tpUserControl->Onoff_2);
#endif
#if(SWITCH_NUM >=3)
  filing_device_data(3,1,&tpUserControl->Onoff_3);
  //report_endpoint_onoff(3,tpUserControl->Onoff_3);
#endif

  report_agreement(filing_data,filing_idx);
 // report_endpoint(1,tpUserControl->Onoff);
//  report_endpoint(2,tpUserControl->Onoff_2);
 // report_endpoint(3,tpUserControl->Onoff_3);
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
          sl_zigbee_app_debug_println("recv attribute onoff =%d , endpoint:%d\r\n",r_temp_onoff, endpoint);
          if(g_endpoint == 0x01){
              if(r_temp_onoff !=tpUserControl->Onoff){
                  tpUserControl->Onoff = r_temp_onoff ;
                  set_write_flag(1);
              }
              report_endpoint(g_endpoint,tpUserControl->Onoff);
          }else  if(g_endpoint == 0x02){
              if(r_temp_onoff !=tpUserControl->Onoff_2){
                  tpUserControl->Onoff_2 = r_temp_onoff ;
                  set_write_flag(1);
              }
              report_endpoint(g_endpoint,tpUserControl->Onoff_2);
          }else  if(g_endpoint == 0x03){
              if(r_temp_onoff !=tpUserControl->Onoff_3){
                  tpUserControl->Onoff_3 = r_temp_onoff ;
                  set_write_flag(1);
              }
              report_endpoint(g_endpoint,tpUserControl->Onoff_3);
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

  status = emberAfPluginFindAndBindInitiatorStart(2);
  sl_zigbee_app_debug_println("2Find and bind initiator %s: 0x%02X", "start", status);

  status = emberAfPluginFindAndBindInitiatorStart(3);
  sl_zigbee_app_debug_println("3Find and bind initiator %s: 0x%02X", "start", status);

}

//----------------------
// Implemented Callbacks
void emberAfMainInitCallback(void)
{
 // sl_zigbee_event_init(&commissioning_event, commissioning_event_handler);
 // sl_zigbee_event_init(&led_event, led_event_handler);
  sl_zigbee_event_init(&finding_and_binding_event, finding_and_binding_event_handler);
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
  if (status == EMBER_NETWORK_DOWN) {
      EmberStatus status = emberAfNetworkState();
      sl_zigbee_app_debug_println("emberAfStackStatusCallback status =%d = EMBER_NETWORK_DOWN\n\n",status);
      if(status == EMBER_NO_NETWORK){
        join_net_work(1);   //掉线重新加网
      }

  //  led_turn_off(COMMISSIONING_STATUS_LED);
  } else if (status == EMBER_NETWORK_UP) {
      report_all_data();
  //  led_turn_on(COMMISSIONING_STATUS_LED);
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
       if(key1_flag){
          key1_flag =0;
          tpUserControl->Onoff ^= 1;
          sl_zigbee_app_debug_println("BUTTON1 onoff 0x%02X", tpUserControl->Onoff);
          report_endpoint(1,tpUserControl->Onoff);
          set_write_flag(1);
       }

    }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
        key1_flag =1;
    }
  } else if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON1) == handle) {  //switch 2
    if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
        if(key2_flag){
          key2_flag =0;
          tpUserControl->Onoff_2 ^= 1;
          sl_zigbee_app_debug_println("BUTTON2 onoff 0x%02X", tpUserControl->Onoff_2);
          report_endpoint(2,tpUserControl->Onoff_2);
          set_write_flag(1);
        }
    }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
        key2_flag =1;
    }
  }else if (SL_SIMPLE_BUTTON_INSTANCE(2) == handle) {
      if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
          if(key3_flag){
            key3_flag =0;
            tpUserControl->Onoff_3 ^= 1;
            sl_zigbee_app_debug_println("BUTTON3 onoff  0x%02X", tpUserControl->Onoff_3);
            report_endpoint(3,tpUserControl->Onoff_3);
            set_write_flag(1);
          }
      }else if( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
          key3_flag =1;
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
