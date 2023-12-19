#include "private_parse.h"
#include "ota-client-policy-config.h"

#define PRIVE_ZUBAO 0


#if PRIVE_ZUBAO

typedef struct {
//  uint8_t  cmd;
  uint8_t   enable;        // 命令序号
  int      seq;        // 命令序号
  uint8_t  all_pack;   // 总包数量
  uint8_t  pack_seq;   // 收到的包序号
  uint8_t  pack_num;   // 收到的包数量
  uint8_t  pack_dat[10][75];   //分包存储
  uint8_t  pack_len[10];       //分包数据长度
  uint8_t  all_dat[750];       //总包存储
  uint16_t offset;             //总包偏移位
} G_PARSE_T; // 解析结构体


#define  BUFF_PARSE   3
G_PARSE_T parse_data[BUFF_PARSE] ={0};
int findDataBySeq(int seq);
int findEmptyData(void);
#endif





uint8_t  send_out[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE]={0};
uint16_t out_idx =0;
static uint8_t seq=0;

void report_hearbeat(void)
{
  uint8_t  out[6] ={0xff,0x99,0x01,0x01,0x00,0x00};
  uint8_t  idx =0;
  out[idx++] = 0xff;
  out[idx++] = 0x99;//
  out[idx++] = seq++;
  out[idx++] = 0x01; // 总包数
  out[idx++] = 0x00; // 包序号
  out[idx++] = EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION;  //版本号

  report_agreement(out,idx);
}



//填充数据包头 传入包体， 将数据包扔出去
void make_gateway_frame(uint8_t cmd,uint8_t num,void *val,uint16_t len)
{
  uint8_t  out[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE]={0};

  uint16_t idx =0;
//  uint16_t  i;
  out[idx++] = 0xff;
  out[idx++] = cmd;
  out[idx++] = seq++;
  out[idx++] = 0x01; // 总包数
  out[idx++] = 0x00; // 包序号
  out[idx++] = num;  //DP数量

  uint8_t *val_ptr = (uint8_t*)val;

  if ((len > 0) && val_ptr) {
      memcpy(out+idx, val_ptr, len);
  }

  len += idx;

   report_agreement(out,len);
}


void report_switch1(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_1_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Onoff_1;

  sl_zigbee_app_debug_println("report Onoff_1 %d \n",tpUserControl->Onoff_1);
}
void report_switch2(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_2_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Onoff_2;

  sl_zigbee_app_debug_println("report Onoff_2 %d \n",tpUserControl->Onoff_2);
}
void report_switch3(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_3_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Onoff_3;

  sl_zigbee_app_debug_println("report Onoff_3 %d \n",tpUserControl->Onoff_3);
}
void report_switch4(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_4_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Onoff_4;

  sl_zigbee_app_debug_println("report Onoff_4 %d \n",tpUserControl->Onoff_4);
}
void report_switch16(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_16_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->backlight_16;

  sl_zigbee_app_debug_println("report backlight_16 %d \n",tpUserControl->backlight_16);
}

#if 0
void report_switch(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_1_SWTICH;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;   //小端
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Onoff;

  sl_zigbee_app_debug_println("report Onoff %d \n",tpUserControl->Onoff);
}


void report_work_mode(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  //uint8_t dat[4]={0};
  send_out[(*idx)++] = DP_2_Work_Mode;
  send_out[(*idx)++] = PROP_ENUM;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = 0x30;	//只有一个页面
 // INT32_TO_LE_BYTES(0x30,dat);

 // sl_zigbee_app_debug_println("report work_mode %d \n",tpUserControl->work_mode);
 // memcpy(send_out+(*idx), dat, 4);
 // (*idx)+=4;
}

void report_lihgt(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =4;
  uint8_t dat[4]={0};
  send_out[(*idx)++] = DP_3_Bright;
  send_out[(*idx)++] = PROP_INT;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;
  INT32_TO_LE_BYTES((tpUserControl->Brightness*10),dat);
  sl_zigbee_app_debug_println("report light %d \n",tpUserControl->Brightness*10);
  memcpy(send_out+(*idx), dat, 4);
  (*idx)+=4;
}


void report_temp(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =4;
  uint8_t dat[4]={0};
  send_out[(*idx)++] = DP_4_TEMP;
  send_out[(*idx)++] = PROP_INT;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;
  INT32_TO_LE_BYTES(tpUserControl->Color_Temp,dat);
  sl_zigbee_app_debug_println("report Color_Temp %d \n",tpUserControl->Color_Temp);

  memcpy(send_out+(*idx), dat, 4);
  (*idx)+=4;
}


void report_hsv(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =6;
  send_out[(*idx)++] = DP_5_COLOUR;
  send_out[(*idx)++] = PROP_RAW;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->Color_H >>8;
  send_out[(*idx)++] = tpUserControl->Color_H;

  send_out[(*idx)++] = tpUserControl->Color_S >>8;
  send_out[(*idx)++] = tpUserControl->Color_S;

  send_out[(*idx)++] = tpUserControl->Color_V >>8;
  send_out[(*idx)++] = tpUserControl->Color_V;

  sl_zigbee_app_debug_println("report hsv %d  %d  %d  \n",tpUserControl->Color_H,tpUserControl->Color_S,tpUserControl->Color_V);
}



void report_countdown(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =4;
  uint8_t dat[4]={0};
  send_out[(*idx)++] = DP_7_COUNTDOWN;
  send_out[(*idx)++] = PROP_INT;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;

  INT32_TO_LE_BYTES(tpUserControl->Count_Down,dat);

  memcpy(send_out+(*idx), dat, 4);
  (*idx)+=4;


  sl_zigbee_app_debug_println("report Count_Down %ld \n",tpUserControl->Count_Down);
}




void report_not_disturb(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =1;
  send_out[(*idx)++] = DP_34_DO_NOT_DISTURB;
  send_out[(*idx)++] = PROP_BOOL;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;
  send_out[(*idx)++] = tpUserControl->not_disturb;

  sl_zigbee_app_debug_println("report not_disturb %d \n",tpUserControl->not_disturb);

}

void report_menmory(uint8_t *send_out,uint16_t* idx)
{
  uint8_t len =12;
  send_out[(*idx)++] = DP_33_POWER_MEMORY;
  send_out[(*idx)++] = PROP_RAW;
  send_out[(*idx)++] = len;
  send_out[(*idx)++] = len>>8;

  send_out[(*idx)++] = 0;   //版本号
  send_out[(*idx)++] = tpUserControl->memory.mode;  //模式

  //hsv
  send_out[(*idx)++] = tpUserControl->memory.Color_H >>8;
  send_out[(*idx)++] = tpUserControl->memory.Color_H;

  send_out[(*idx)++] = tpUserControl->memory.Color_S >>8;
  send_out[(*idx)++] = tpUserControl->memory.Color_S;

  send_out[(*idx)++] = tpUserControl->memory.Color_V >>8;
  send_out[(*idx)++] = tpUserControl->memory.Color_V;

  uint16_t  Brightness = tpUserControl->memory.Brightness*10;
  send_out[(*idx)++] = Brightness >>8;
  send_out[(*idx)++] = Brightness;


  uint16_t  Color_Temp = ((tpUserControl->memory.Color_Temp -2700)*10) /38;
  send_out[(*idx)++] = Color_Temp >>8;
  send_out[(*idx)++] = Color_Temp;


  sl_zigbee_app_debug_println("report menmory %d  HSV  %d  %d  %d  light %d  temp %d  \n",tpUserControl->memory.mode,tpUserControl->memory.Color_H,
         tpUserControl->memory.Color_S,tpUserControl->memory.Color_V,tpUserControl->memory.Brightness,tpUserControl->memory.Color_Temp);

}

void report_cmd_countdown(void)
{
  uint8_t  send_out[15]={0};
  uint16_t idx =0;

  report_countdown(send_out,&idx);
  make_gateway_frame(CMD_DEVICE_REPORT_STATUS,1,send_out,idx);
}
#endif

void report_all_data(void)
{
  if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
      sl_zigbee_app_debug_println("report_all_data no network return report:%d\r\n");
      return;
  }
  uint8_t  send_out[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE]={0};
  uint16_t idx =0;
  uint8_t  dp_num =0;
  //报开关
#if(SWITCH_NUM >=1)
    report_switch1(send_out,&idx);
    dp_num ++;
#endif
#if(SWITCH_NUM >=2)
    report_switch2(send_out,&idx);
    dp_num ++;
#endif
#if(SWITCH_NUM >=3)
    report_switch3(send_out,&idx);
    dp_num ++;
#endif
#if(SWITCH_NUM >=4)
    report_switch4(send_out,&idx);
    dp_num ++;
#endif
    report_switch16(send_out,&idx);
    dp_num ++;
//  report_switch(send_out,&idx);
//  dp_num ++;
//  report_work_mode(send_out,&idx);
//  dp_num ++;
//  report_lihgt(send_out,&idx);
//  dp_num ++;
 // report_temp(send_out,&idx);
//  dp_num ++;
//  report_hsv(send_out,&idx);
//  dp_num ++;
//  report_countdown(send_out,&idx);
//  dp_num ++;
//  report_menmory(send_out,&idx);
//  dp_num ++;
//  report_not_disturb(send_out,&idx);
//  dp_num ++;

  sl_zigbee_app_debug_println("report_all_data start: len %d %d \r\n",idx,dp_num);
  make_gateway_frame(CMD_DEVICE_REPORT_STATUS,dp_num,send_out,idx);
#if 0
  switch()
  {
    case PROP_BOOL:    //1个字节
      break;
    case PROP_INT:
    break;
    case PROP_FLOAT:
      break;
    case PROP_ENUM:
      break;
    case PROP_FAULT:
      break;
    case PROP_TEXT:
      break;
    case PROP_RAW:
      break;
    case PROP_STRUCT:
      break;
    case PROP_ARRAY:
      break;
    default:break;
  }
#endif
}



void make_rec_report(uint8_t cmd, void *val,uint16_t len)
{
#if 1
  uint8_t  send_out[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE]={0};
  uint16_t idx =0;
  uint16_t  i,j;
  send_out[idx++] = 0xff;
  send_out[idx++] = cmd;
  send_out[idx++] = seq++;
  //  val_ptr[idx++] = 0x01; // 总包数
  //  val_ptr[idx++] = 0x01; // 包序号   //只修改包的这部分内容， 其他的找原数据发出去
    idx++;
    idx++;
  j=idx;
  uint8_t *val_ptr = (uint8_t*)val;
  for (i = j; i < (len); i++) {
      send_out[idx++] = val_ptr[i]; // 解引用指针取得数据
    //  sl_zigbee_app_debug_println(" rport val_ptr[i]: 0x%02X\n",val_ptr[i]);
  }

  report_agreement(send_out,idx);
#else
  uint8_t *val_ptr = (uint8_t*)val;
  uint16_t idx =0;

  val_ptr[idx++] = 0xff;
  val_ptr[idx++] = cmd;
  val_ptr[idx++] = seq++;
//  val_ptr[idx++] = 0x01; // 总包数
//  val_ptr[idx++] = 0x01; // 包序号   //只修改包的这部分内容， 其他的找原数据发出去
  idx++;
  idx++;


  sl_zigbee_app_debug_println("recv report :\n");
  for (int i = 0; i < (len); i++) {
     //  send_out[idx++] = val_ptr[i]; // 解引用指针取得数据
    sl_zigbee_app_debug_print(" 0x%02X",val_ptr[i]);
  }
  sl_zigbee_app_debug_println("\n");
  report_agreement(send_out,len);
#endif
}


//设置随机数种子
void set_rand_source(void)
{
  uint16_t SHORT_ID =  emberAfGetNodeId();
  srand(SHORT_ID);
  sl_zigbee_app_debug_println("SHORT_ID =0x%2x   \n",SHORT_ID);
//  EmberEUI64 eui;
//  emberAfGetEui64(eui);
//  emberAfPrintBigEndianEui64(eui);
}


// 解析函数
uint16_t HeartbeatTime =0;
uint32_t Save_HbTime =0;
void parseProtocol(uint8_t *data, uint16_t length) {
    // 检查包头
    static uint8_t init_srand =0;
    // 获取命令字

    uint8_t  send_out[512]={0};
    uint8_t  dp_count =0;
    uint16_t idx =0;

    uint8_t cmd = data[1];

    uint8_t i = 0;
  //  uint8_t x=0, j = 0;
    uint8_t dpNum = 0;
    uint8_t dpid_dp = 0;
    uint16_t dpid_len = 0;
    uint8_t type = 0;
    uint8_t dataIndex = 0;

 //   uint8_t switch_num = 0; //节点数量
   // uint8_t rhythm_onoff =0;  //节律开关
  //  uint16_t temp = 0; //
    switch (cmd) {
     //   case CMD_HEARTBEAT:
        case CMD_DEVICE_PING:
            sl_zigbee_app_debug_println("Received Heartbeat\n");
            dataIndex =5; //头部信息不要
            //小端
            if(!init_srand){
                init_srand =1;
                set_rand_source();
                report_all_data();    //第一次ping 上报设备状态
            }
          //  uint32_t utc_temp = (uint32_t)((data[dataIndex]) | (data[dataIndex + 1] << 8) | (data[dataIndex + 2] << 16) | (data[dataIndex + 3] << 24));
            dataIndex+=4;
            uint8_t Divisor = data[dataIndex++];
            uint8_t Remainder = data[dataIndex++];
            uint8_t Max = data[dataIndex++];
            uint8_t BaseTime = data[dataIndex++];
            uint16_t SHORT_ID =  emberAfGetNodeId();  //设备短地址
            sl_zigbee_app_debug_println("Divisor %d SHORT_ID =0x%2x  BaseTime %d Max %d\n",Divisor,SHORT_ID,BaseTime,Max);

            sl_zigbee_app_debug_println("(SHORT_ID '%' Divisor) =%d  Remainder %d \n",(SHORT_ID % Divisor),Remainder);
            if((SHORT_ID % Divisor) == Remainder){
                HeartbeatTime =BaseTime * (rand()%(Max+1));
                if(!HeartbeatTime){
                    report_hearbeat();
                }else{
                    Save_HbTime  = halCommonGetInt32uMillisecondTick();
                    sl_zigbee_app_debug_println(" ===  HeartbeatTime %d  Save_HbTime =%d \n",HeartbeatTime,Save_HbTime);
                }
            }

            break;


        case CMD_GATEWAY_COMMAND:
            sl_zigbee_app_debug_println("Received CMD_GATEWAY_COMMAND\n");
            // 获取DP数量
            dataIndex = 5;
            dpNum = data[dataIndex++];
            // 从第7个字节开始解析DP数据
            for (i = 0; i < dpNum; i++) {

                dpid_dp = data[dataIndex++];
                type    = data[dataIndex++];
                dpid_len = ((data[dataIndex+1] << 8) | data[dataIndex]);
                dataIndex += 2;

                sl_zigbee_app_debug_println("DPID: %d, type %d Len: %d,i %d  daNum %d  Data: \n", dpid_dp,type, dpid_len,i,dpNum);

                // 处理dp数据
             //   for (j = 0; j < dpid_len; j++) {
                    switch(dpid_dp)
                    {
                      case DP_1_SWTICH:
                       tpUserControl->Onoff_1 = data[dataIndex++];

                       sl_zigbee_app_debug_println("SWTICH:  %d\n",tpUserControl->Onoff_1);
                        break;
                      case DP_2_SWTICH:
                       tpUserControl->Onoff_2 = data[dataIndex++];

                       sl_zigbee_app_debug_println("SWTICH2:  %d\n",tpUserControl->Onoff_2);
                        break;
                      case DP_3_SWTICH:
                       tpUserControl->Onoff_3 = data[dataIndex++];

                       sl_zigbee_app_debug_println("SWTICH3:  %d\n",tpUserControl->Onoff_3);
                        break;
                      case DP_4_SWTICH:
                       tpUserControl->Onoff_4 = data[dataIndex++];

                       sl_zigbee_app_debug_println("SWTICH4:  %d\n",tpUserControl->Onoff_4);
                        break;
                      case DP_16_SWTICH:
                       tpUserControl->backlight_16 = data[dataIndex++];

                       sl_zigbee_app_debug_println("SWTICH16:  %d\n",tpUserControl->backlight_16);
                        break;
#if 0
                      case DP_2_Work_Mode:
                        tpUserControl->work_mode= data[dataIndex++];

                       sl_zigbee_app_debug_println("Work_Mode:  %d\n",tpUserControl->work_mode);
                        break;

                      case DP_3_Bright:
                   //     tpUserControl->led_mode  =0;
                        //小端
                        tpUserControl->Brightness = ((uint32_t)(data[dataIndex] | (data[dataIndex+1] << 8) | (data[dataIndex+2] << 16)) | (data[dataIndex+3] << 24))/10;
                        dataIndex+=4;
                        execute_led_change_cal(300);
                        sl_zigbee_app_debug_println("Brightness:    %d, \n",tpUserControl->Brightness);
                        break;
                      case DP_4_TEMP:
               //         tpUserControl->led_mode  =0;
                        //小端
                        tpUserControl->Color_Temp = ((uint32_t)(data[dataIndex] | (data[dataIndex+1] << 8) | (data[dataIndex+2] << 16)) | (data[dataIndex+3] << 24));
                        dataIndex+=4;
               //         sl_zigbee_app_debug_println("TEMP:    %d, LED %d \n",  tpUserControl->Color_Temp , tpUserControl->led_mode);

              //          execute_led_change_cal(300);

                        break;

                      case DP_5_COLOUR:

                        tpUserControl->led_mode  =1;
                        tpUserControl->Color_H = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                        dataIndex += 2;
                        tpUserControl->Color_S = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                        dataIndex += 2;
                        tpUserControl->Color_V = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                        dataIndex += 2;

                        sl_zigbee_app_debug_println("COLOR:   H %d, S %d V: %d \n",  tpUserControl->Color_H , tpUserControl->Color_S ,  tpUserControl->Color_V);
                        execute_led_change_cal(500);
                        break;
                      case DP_6_SCENE:
                        switch(data[dataIndex++]){
                          case 0x00: //晚安
                            tpUserControl->scen_mode = 1;
                            break;
                          case 0x01: //阅读
                            tpUserControl->scen_mode = 2;
                            break;
                          case 0x02: //工作
                            tpUserControl->scen_mode = 3;
                            break;
                          case 0x03: //休闲
                            tpUserControl->scen_mode = 4;
                            break;
                          case 0x18: //向日葵
                            tpUserControl->scen_mode = 6;
                            break;
                          case 0x04: //草原
                            tpUserControl->scen_mode = 7;
                            break;
                          case 0x06: //炫彩
                            tpUserControl->scen_mode = 8;
                            break;
                          case 0x17: //海洋
                            tpUserControl->scen_mode = 5;
                            break;
                          default:
                            break;
                        }
                        count_100ms =40;
                        set_scen_hsv_led_mode(tpUserControl->scen_mode);
                        set_scen_cw_led_mode(tpUserControl->scen_mode);
                        dataIndex += 26;

                        sl_zigbee_app_debug_println("scen_mode:  %d\n",tpUserControl->scen_mode);
                        break;
                      case DP_7_COUNTDOWN:

                        tpUserControl->Count_Down = ((uint32_t)(data[dataIndex] | (data[dataIndex+1] << 8) | (data[dataIndex+2] << 16)) | (data[dataIndex+3] << 24));

                        dataIndex+=4;
                        sl_zigbee_app_debug_println("Count_Down:  %ld\n",tpUserControl->Count_Down);
                        break;
                      case DP_9_CONTROL:
                      dataIndex+=21;
                        break;
                #if  0
                      case DP_30_RHYTHM:
                        dataIndex++;    //版本号扔掉

                        dataIndex++;  //任务开关  8
                        dataIndex++;  //变化模式  9
                        dataIndex++;  //日期设定  10
                        switch_num = data[dataIndex++];   //节点个数

                        for(x=0;x<switch_num;x++){
                            tpUserControl->bio_rhythm.onoff[x] = data[8];   //节点任务开关
                            tpUserControl->bio_rhythm.week[x][0] = data[10]; //任务日期设定
                            dataIndex++;  //节点开关  扔掉
                         //   tpUserControl->bio_rhythm.execution_time[x] = 300000 * data[dataIndex++]; //渐变时间设定 1=5分钟， 2=十分钟 ...
                            tpUserControl->bio_rhythm.hours[x] = data[dataIndex++];   //执行的小时 时间点
                            tpUserControl->bio_rhythm.minutes[x] = data[dataIndex++]; //执行的分钟 时间点
                            if(data[dataIndex + 3] ==0){                          //明度V =0， 代表非彩光数据
                                dataIndex += 4; //跳到亮度百分比的位置去
                                tpUserControl->bio_rhythm.led_mode[x]  =0;
                                tpUserControl->bio_rhythm.mode_data[x][2] = data[dataIndex++];  //亮度
                                temp = 2700 + (38* data[dataIndex++]);   //色温值
                                tpUserControl->bio_rhythm.mode_data[x][0] = temp >>8;  //色温高位
                                tpUserControl->bio_rhythm.mode_data[x][1] = temp;  //色温低位
                            }else{
                                tpUserControl->bio_rhythm.led_mode[x]  =1;
                                temp = (data[dataIndex]*100 +  data[dataIndex +1]);   //色调百位 和十位个位
                                tpUserControl->bio_rhythm.mode_data[x][0] = temp >>8;  //h_H
                                tpUserControl->bio_rhythm.mode_data[x][1] = temp;      //h_L
                                dataIndex+=2;

                                tpUserControl->bio_rhythm.mode_data[x][2] = data[dataIndex++];  //s
                                tpUserControl->bio_rhythm.mode_data[x][3] = data[dataIndex++];  //v
                                dataIndex+=2;   //加掉色温 亮度 的数据位置
                            }
                        }

                        break;
            #endif

            #if  0
                      case DP_31_ASLEEP:
                        dataIndex++;    //版本号扔掉
                        switch_num = data[dataIndex++]; //节点数量
                        for(x=0;x<switch_num;x++){
                            tpUserControl->asleep.onoff[x] = data[dataIndex++]; //节点任务开关
                            tpUserControl->asleep.week[x][0] = data[dataIndex++]; //任务日期设定
                            tpUserControl->asleep.execution_time[x] = 300000 * data[dataIndex++]; //渐变时间设定 1=5分钟， 2=十分钟 ...
                            tpUserControl->asleep.hours[x] = data[dataIndex++]; //执行的小时 时间点
                            tpUserControl->asleep.minutes[x] = data[dataIndex++]; //执行的分钟 时间点
                            if(data[dataIndex + 3] ==0){                          //明度V =0， 代表非彩光数据
                                dataIndex += 4; //跳到亮度百分比的位置去
                                tpUserControl->asleep.led_mode[x]  =0;
                                tpUserControl->asleep.mode_data[x][2] = data[dataIndex++];  //亮度
                                temp = 2700 + (38* data[dataIndex++]);   //色温值
                                tpUserControl->asleep.mode_data[x][0] = temp >>8;  //色温高位
                                tpUserControl->asleep.mode_data[x][1] = temp;  //色温低位
                            }else{
                                tpUserControl->asleep.led_mode[x]  =1;
                                temp = (data[dataIndex]*100 +  data[dataIndex +1]);   //色调百位 和十位个位
                                tpUserControl->asleep.mode_data[x][0] = temp >>8;  //h_H
                                tpUserControl->asleep.mode_data[x][1] = temp;      //h_L
                                dataIndex+=2;

                                tpUserControl->asleep.mode_data[x][2] = data[dataIndex++];  //s
                                tpUserControl->asleep.mode_data[x][3] = data[dataIndex++];  //v
                                dataIndex+=2;   //加掉色温 亮度 的数据位置
                            }
                        }
                        break;
                      case DP_32_WAKEUP:
                        dataIndex++;    //版本号扔掉
                        switch_num = data[dataIndex++]; //节点数量
                        for(x=0;x<switch_num;x++){
                            tpUserControl->awaken.onoff[x] = data[dataIndex++]; //节点任务开关
                            tpUserControl->awaken.week[x][0] = data[dataIndex++]; //任务日期设定
                            tpUserControl->awaken.execution_time[x] = 300000 * data[dataIndex++]; //渐变时间设定 1=5分钟， 2=十分钟 ...
                            tpUserControl->awaken.hours[x] = data[dataIndex++]; //执行的小时 时间点
                            tpUserControl->awaken.minutes[x] = data[dataIndex++]; //执行的分钟 时间点
                            if(data[dataIndex + 3] ==0){                          //明度V =0， 代表非彩光数据
                                dataIndex += 4; //跳到亮度百分比的位置去
                                tpUserControl->awaken.led_mode[x]  =0;
                                tpUserControl->awaken.mode_data[x][2] = data[dataIndex++];  //亮度
                                temp = 2700 + (38* data[dataIndex++]);   //色温值
                                tpUserControl->awaken.mode_data[x][0] = temp >>8;  //色温高位
                                tpUserControl->awaken.mode_data[x][1] = temp;  //色温低位
                            }else{
                                tpUserControl->awaken.led_mode[x]  =1;
                                temp = (data[dataIndex]*100 +  data[dataIndex +1]);   //色调百位 和十位个位
                                tpUserControl->awaken.mode_data[x][0] = temp >>8;  //h_H
                                tpUserControl->awaken.mode_data[x][1] = temp;      //h_L
                                dataIndex+=2;

                                tpUserControl->awaken.mode_data[x][2] = data[dataIndex++];  //s
                                tpUserControl->awaken.mode_data[x][3] = data[dataIndex++];  //v
                                dataIndex+=2;   //加掉色温 亮度 的数据位置
                            }
                            tpUserControl->awaken.continue_time[x] =  data[dataIndex++]*5;  //状态维持时间  5分钟一个步进值

                        }

                        break;
            #endif
                      case DP_33_POWER_MEMORY:
                        dataIndex++;    //版本号扔掉
                        tpUserControl->memory.mode = data[dataIndex++];  //模式


                        if(tpUserControl->memory.mode == 2){
                            tpUserControl->memory.Color_H = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                            dataIndex += 2;
                            tpUserControl->memory.Color_S = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                            dataIndex += 2;
                            tpUserControl->memory.Color_V = ((data[dataIndex] << 8) | data[dataIndex + 1]);
                            dataIndex += 2;



                            tpUserControl->memory.Brightness = ((data[dataIndex] << 8) | data[dataIndex + 1])/10;
                            dataIndex += 2;
                            tpUserControl->memory.Color_Temp = 2700 + (38*((data[dataIndex] << 8) | data[dataIndex + 1])/10);
                            dataIndex += 2;
                            if((tpUserControl->memory.Color_Temp ==0) && (tpUserControl->memory.Brightness==0)){   //hsv 数据
                                tpUserControl->memory.led_mode = 1;
                            }else{      //色温  数据
                                tpUserControl->memory.led_mode = 0;
                            }
                            if(tpUserControl->memory.Brightness == 0){
                                tpUserControl->memory.Brightness =50;
                            }
                        }

                        sl_zigbee_app_debug_println("POWER_MEMORY: mode %d led_mode %d  hsv %d %d %d liht %d  temp %d\n",tpUserControl->memory.mode,tpUserControl->memory.led_mode,  tpUserControl->memory.Color_H,
                               tpUserControl->memory.Color_S,tpUserControl->memory.Color_V,tpUserControl->memory.Brightness,tpUserControl->memory.Color_Temp);
                        break;
                      case DP_34_DO_NOT_DISTURB:
                        tpUserControl->not_disturb = data[dataIndex++];
                        sl_zigbee_app_debug_println("DP_34_DO_NOT_DISTURB:  %d\n",tpUserControl->not_disturb);
                        break;
                      case DP_35_SWTICH_GRADIENT:

                        dataIndex++;    //版本号扔掉

                        tpUserControl->On_buff =data[dataIndex++];
                        tpUserControl->Off_buff =data[dataIndex++];
                        break;
#endif
                      default:break;
                    }
          //      }


            }

            //把收到的数据改个命令字直接回过去
            make_rec_report(CMD_DEVICE_REPORT_STATUS,data,length);
            set_write_flag(1);
            break;


        case CMD_DEVICE_STATUS_QUERY:
                      // 从第7个字节开始解析DP数据
          dataIndex = 5;
          dpNum = data[dataIndex++];
          if(dpNum == 0){ //上报所有状态
              //上报所有DP点
              #if(SWITCH_NUM >=1)
                                report_switch1(send_out,&idx);
                                dp_count ++;
              #endif
              #if(SWITCH_NUM >=2)
                                report_switch2(send_out,&idx);
                                dp_count ++;
              #endif
              #if(SWITCH_NUM >=3)
                                report_switch3(send_out,&idx);
                                dp_count ++;
              #endif
              #if(SWITCH_NUM >=4)
                                report_switch4(send_out,&idx);
                                dp_count ++;
              #endif
                                report_switch16(send_out,&idx);
                                dp_count ++;
          }
          for (i = 0; i < dpNum; i++) {
              dpid_dp = data[dataIndex++];
              sl_zigbee_app_debug_println("DPID: %d \n", dpid_dp);
              // 处理dp数据
              switch(dpid_dp)
              {
                case DP_1_SWTICH:
                  report_switch1(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH1:  %d\n",tpUserControl->Onoff_1);
                  break;
                case DP_2_SWTICH:
                  report_switch2(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH2:  %d\n",tpUserControl->Onoff_2);
                  break;
                case DP_3_SWTICH:
                  report_switch3(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH3:  %d\n",tpUserControl->Onoff_3);
                  break;
                case DP_4_SWTICH:
                  report_switch4(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH4:  %d\n",tpUserControl->Onoff_4);
                  break;
                case DP_16_SWTICH:
                  report_switch16(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH16:  %d\n",tpUserControl->backlight_16);
                  break;
#if 0
                case DP_1_SWTICH:
                  report_switch(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("SWTICH:  %d\n",tpUserControl->Onoff);
                  break;
                case DP_2_Work_Mode:
                  report_work_mode(send_out,&idx);
                  dp_count ++;
             //    sl_zigbee_app_debug_println("Work_Mode:  %d\n",tpUserControl->work_mode);
                  break;
                case DP_3_Bright:
                  report_lihgt(send_out,&idx);
                  dp_count ++;
                  sl_zigbee_app_debug_println("Brightness:    %d, \n",tpUserControl->Brightness);
                  break;
                case DP_4_TEMP:
               //   report_temp(send_out,&idx);
               //   dp_count ++;
               //   sl_zigbee_app_debug_println("TEMP:    %d, LED %d \n",  tpUserControl->Color_Temp , tpUserControl->led_mode);
                  break;
                case DP_5_COLOUR:
               //   report_hsv(send_out,&idx);
               //   dp_count ++;
               //   sl_zigbee_app_debug_println("COLOR:   H %d, S %d V: %d \n",  tpUserControl->Color_H , tpUserControl->Color_S ,  tpUserControl->Color_V);
                  break;
                case DP_6_SCENE:
               //   sl_zigbee_app_debug_println("scen_mode:  %d\n",tpUserControl->scen_mode);
                  break;
                case DP_7_COUNTDOWN:
               //   report_countdown(send_out,&idx);
               //   dp_count ++;
               //   sl_zigbee_app_debug_println("Count_Down:  %ld\n",tpUserControl->Count_Down);
                  break;
                case DP_9_CONTROL:
                  break;
                case DP_30_RHYTHM:
                  break;
                case DP_31_ASLEEP:
                  break;
                case DP_32_WAKEUP:
                  break;
                case DP_33_POWER_MEMORY:
                //  report_menmory(send_out,&idx);
                  dp_count ++;
                  break;
                case DP_34_DO_NOT_DISTURB:
                //  report_not_disturb(send_out,&idx);
                  dp_count ++;
                  break;
                case DP_35_SWTICH_GRADIENT:
                  break;
#endif
                case 0xff:  //上报所有DP点
#if(SWITCH_NUM >=1)
                  report_switch1(send_out,&idx);
                  dp_count ++;
#endif
#if(SWITCH_NUM >=2)
                  report_switch2(send_out,&idx);
                  dp_count ++;
#endif
#if(SWITCH_NUM >=3)
                  report_switch3(send_out,&idx);
                  dp_count ++;
#endif
#if(SWITCH_NUM >=4)
                  report_switch4(send_out,&idx);
                  dp_count ++;
#endif
                  report_switch16(send_out,&idx);
                  dp_count ++;
                //  report_switch(send_out,&idx);
                //  report_work_mode(send_out,&idx);
                //  report_lihgt(send_out,&idx);
                //  report_temp(send_out,&idx);
                //  report_hsv(send_out,&idx);
                //  report_countdown(send_out,&idx);
                //  report_menmory(send_out,&idx);
                //  report_not_disturb(send_out,&idx);
                //  dp_count =3;
                  break;
                default: break;
              }
           }

          sl_zigbee_app_debug_println("report_all_data start: len %d\r\n",idx);
          make_gateway_frame(CMD_DEVICE_REPORT_STATUS,dp_count,send_out,idx);
            break;
        // 处理其他命令
        default:

            sl_zigbee_app_debug_println("Unknown command: 0x%02X\n", cmd);
            break;
    }
}





//私有协议解析入口
void zigbee_Private_Trasmit_Parse(uint8_t endpoint,
                                  EmberAfClusterId cluster,
                                  EmberAfAttributeId attributeID,
                                  uint8_t *dataPtr,
                                  uint16_t readLength)
{

 // uint8_t *data =  dataPtr;
 // uint8_t all_pack=0,pack_seq=0,cmd_seq=0;
//
//  sl_zigbee_app_debug_println("endpoint=%x  cluster=%x  attributeID=%x  readLength =%d \n",endpoint,cluster,attributeID,readLength);

 //   cmd_seq = data[3];    //数据中第一位为总长度，所有从3开始，
 //   all_pack = data[4];
  //  pack_seq = data[5];

//    sl_zigbee_app_debug_println("cmd_seq = %d all_pack = %d  pack_seq = %d  ", cmd_seq, all_pack, pack_seq);
    return;    //低功耗不进入接收
    parseProtocol(dataPtr, readLength); //直接将拿到的数据进行解析
    //字节定义的重组包协议无需用到
#if PRIVE_ZUBAO
    if(all_pack == 0){ //无分包的情况
        parseProtocol(dataPtr, readLength); //直接将拿到的数据进行解析
 //       sl_zigbee_app_debug_println("no pack\n");
    } else {
        int seq=-1;
        seq = findDataBySeq(cmd_seq);
        if (seq == -1) {//新seq  没有匹配到任何记录里
            seq = findEmptyData();
            if (seq == -1) {
                sl_zigbee_app_debug_println("store full\n");
                return;
            }
            parse_data[seq].seq = cmd_seq;
            parse_data[seq].enable = 1;
        }
        //有匹配到  或者有空的存储位可以用，将数据存下来

   //     sl_zigbee_app_debug_println("start fenbao seq = %d \n", seq);
        memset(parse_data[seq].pack_dat[pack_seq], 0, 75);
        if (pack_seq) {//非第一包，去掉6位头码
            readLength -= 6;                           //将数据去掉头码放置到分包数组中
            memcpy(parse_data[seq].pack_dat[pack_seq], (uint8_t*) &dataPtr[6], (readLength));
        } else {                                       //将所有数据放置到分包数组中
            memcpy(parse_data[seq].pack_dat[pack_seq], dataPtr, readLength);
        }

        //将每包的长度记忆下来
        parse_data[seq].pack_len[pack_seq] = readLength;

    //    sl_zigbee_app_debug_println("pack_num = %d  pack_len = %d \n", parse_data[seq].pack_num, parse_data[seq].pack_len[pack_seq]);

        //全部包接收完成， 开始组包然后发送去解析。
        if (parse_data[seq].pack_num == all_pack) {
            parse_data[seq].offset = 0;                 //保证组包从头开始

            for (uint8_t i = 0; i <= all_pack; i++) {
                //按顺序将分包按位置 放入总包中
                memcpy((uint8_t*) parse_data[seq].all_dat + parse_data[seq].offset,
                       (uint8_t*) parse_data[seq].pack_dat[i],
                       parse_data[seq].pack_len[i]);

                parse_data[seq].offset += (parse_data[seq].pack_len[i]);
             //   sl_zigbee_app_debug_println("offset = %d \n", parse_data[seq].offset);
            }

            //拼包完成， 发出去解析
            sl_zigbee_app_debug_println("seq :%d  pack_num: %d  offset: %d \n",seq, parse_data[seq].pack_num, parse_data[seq].offset);
            parseProtocol(parse_data[seq].all_dat, parse_data[seq].offset); //直接将拿到的数据进行解析
            //清空该数据包的相关参数，使得下次可以正常使用
            parse_data[seq].pack_num = 0;
            parse_data[seq].offset = 0;
            parse_data[seq].enable = 0;
            parse_data[seq].seq = -1;
        }else{
            //计数接收处理的包数量
            parse_data[seq].pack_num++;
        }
    }
#endif
}



#if PRIVE_ZUBAO
//查找有没有匹配的 已经存了序列号的数组
int findDataBySeq(int seq) {
    for (int i = 0; i < BUFF_PARSE; i++) {
        if (parse_data[i].enable && parse_data[i].seq == seq) {
            return i;
        }
    }
    return -1; // 未找到匹配的数组
}


//查找没有被使用的序号
int findEmptyData(void) {
    for (int i = 0; i < BUFF_PARSE; i++) {
        if (!parse_data[i].enable) {
            return i;
        }
    }
    return -1; // 所有数组都被分配
}
#endif





































