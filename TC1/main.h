#ifndef __MAIN_H_
#define __MAIN_H_

#include "mico.h"
#include "micokit_ext.h"

#define VERSION "v0.10.1"

#define TYPE 1
#define TYPE_NAME "zTC1"

#define ZTC1_NAME "zTC1-%s"

#define USER_CONFIG_VERSION 2
#define SETTING_MQTT_STRING_LENGTH_MAX 32 //必须4字节对齐。

#define SOCKET_NAME_LENGTH   32
#define SOCKET_NUM           6  //插座数量
#define SOCKET_TIME_TASK_NUM 5  //每个插座最多5组定时任务

#define Led    MICO_GPIO_5
#define Button MICO_GPIO_23
#define POWER  MICO_GPIO_15

#define Relay_ON  1
#define Relay_OFF 0

#define Relay_0   MICO_GPIO_6
#define Relay_1   MICO_GPIO_8
#define Relay_2   MICO_GPIO_10
#define Relay_3   MICO_GPIO_7
#define Relay_4   MICO_GPIO_9
#define Relay_5   MICO_GPIO_18
#define Relay_NUM SOCKET_NUM

typedef struct
{
    char hour;   //小时
    char minute; //分钟
    char repeat; //bit7:一次 bit6-0:周日-周一
    char action; //动作
    char on;     //开关
} user_socket_task_config_t;

typedef struct
{
    char name[SOCKET_NAME_LENGTH];
    char on;    //记录当前开关
    user_socket_task_config_t task[SOCKET_TIME_TASK_NUM];
} user_socket_config_t;

//用户保存参数结构体
typedef struct
{
    char mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];        //mqtt service ip
    int mqtt_port;                                       //mqtt service port
    char mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];      //mqtt service user
    char mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];  //mqtt service user
//  char mqtt_device_id[SETTING_MQTT_STRING_LENGTH_MAX]; //mqtt service user device name
    char version;
    user_socket_config_t socket[SOCKET_NUM];
} user_config_t;

extern char rtc_init;
extern uint32_t total_time;
extern char strMac[16];
extern char str_mac[16];
extern uint32_t power;
extern system_config_t* sys_config;
extern user_config_t* user_config;
extern char socket_status[32];
extern mico_gpio_t Relay[Relay_NUM];

#endif
