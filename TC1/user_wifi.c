#include "user_wifi.h"

#include "main.h"
#include "mico_socket.h"
#include "user_gpio.h"
#include "user_sntp.h"
#include "user_function.h"

#define os_log(format, ...)  custom_log("WIFI", format, ##__VA_ARGS__)

char wifi_status = WIFI_STATE_NOCONNECT;

mico_timer_t wifi_led_timer;

/*
static void wifi_connect_sys_config(void)
{
    if (strlen(sys_config->micoSystemConfig.ssid) > 0)
    {
        os_log("connect ssid:%s key:%s",sys_config->micoSystemConfig.ssid,sys_config->micoSystemConfig.user_key);
        network_InitTypeDef_st wNetConfig;
        memset(&wNetConfig, 0, sizeof(network_InitTypeDef_st));
        strcpy(wNetConfig.wifi_ssid, sys_config->micoSystemConfig.ssid);
        strcpy(wNetConfig.wifi_key, sys_config->micoSystemConfig.user_key);
        wNetConfig.wifi_mode = Station;
        wNetConfig.dhcpMode = DHCP_Client;
        wNetConfig.wifi_retry_interval = 6000;
        micoWlanStart(&wNetConfig);
        wifi_status = WIFI_STATE_CONNECTING;
    } else
        wifi_status = WIFI_STATE_FAIL;
}
*/

//wifi已连接获取到IP地址 回调
static void wifi_get_ip_callback(IPStatusTypedef *pnet, void * arg)
{
    os_log("got IP:%s", pnet->ip);
    wifi_status = WIFI_STATE_CONNECTED;
    user_function_cmd_received(1,"{\"cmd\":\"device report\"}");
}

//wifi连接状态改变回调
static void wifi_status_callback(WiFiEvent status, void* arg)
{
    if (status == NOTIFY_STATION_UP) //wifi连接成功
    {
        //user_config->last_wifi_status = status;
        sys_config->micoSystemConfig.reserved = status;
        mico_system_context_update(sys_config);

        OSStatus status = micoWlanSuspendSoftAP(); //关闭AP
        if (status != kNoErr)
        {
            os_log("close ap error[%d]", status);
        }

        //wifi_status = WIFI_STATE_CONNECTED;
    }
    else if (status == NOTIFY_STATION_DOWN) //wifi断开
    {
        //user_config->last_wifi_status = status;
        sys_config->micoSystemConfig.reserved = status;
        mico_system_context_update(sys_config);

        ap_init(); //打开AP

        wifi_status = WIFI_STATE_NOCONNECT;
        if (!mico_rtos_is_timer_running(&wifi_led_timer))
        {
            mico_rtos_start_timer(&wifi_led_timer);
        }
    }
}

//100ms定时器回调
static void wifi_led_timer_callback(void* arg)
{
    static unsigned int num = 0;
    num++;

    switch (wifi_status)
    {
        case WIFI_STATE_FAIL:
            os_log("wifi connect fail");
            user_led_set(0);
            mico_rtos_stop_timer(&wifi_led_timer);
            break;
        case WIFI_STATE_NOCONNECT:
            //wifi_connect_sys_config();
            break;
        case WIFI_STATE_CONNECTING:
            num = 0;
            user_led_set(-1);
            break;
        case WIFI_STATE_CONNECTED:
            user_led_set(0);
            mico_rtos_stop_timer(&wifi_led_timer);
            if (relay_out())
                user_led_set(1);
            else
                user_led_set(0);
            break;
    }
}

void wifi_connect(char* wifi_ssid, char* wifi_key)
{
    //wifi配置初始化
    network_InitTypeDef_st wNetConfig;

    memset(&wNetConfig, 0, sizeof(network_InitTypeDef_st));
    wNetConfig.wifi_mode = Station;
    snprintf(wNetConfig.wifi_ssid, 32, wifi_ssid);
    strcpy((char*)wNetConfig.wifi_key, wifi_key);
    wNetConfig.dhcpMode = DHCP_Client;
    wNetConfig.wifi_retry_interval = 6000;
    micoWlanStart(&wNetConfig);

    //保存wifi及密码到Flash
    strcpy(sys_config->micoSystemConfig.ssid, wifi_ssid);
    strcpy(sys_config->micoSystemConfig.user_key, wifi_key);
    sys_config->micoSystemConfig.user_keyLength = strlen(wifi_key);
    mico_system_context_update(sys_config);
    wifi_status = WIFI_STATE_NOCONNECT;
}

void wifi_init(void)
{
    //wifi状态下led闪烁定时器初始化
    mico_rtos_init_timer(&wifi_led_timer, 100, (void *) wifi_led_timer_callback, NULL);
    //wifi已连接获取到IP地址 回调
    mico_system_notify_register(mico_notify_DHCP_COMPLETED, (void *) wifi_get_ip_callback, NULL);
    //wifi连接状态改变回调
    mico_system_notify_register(mico_notify_WIFI_STATUS_CHANGED, (void*) wifi_status_callback, NULL);
    //sntp_init();
    //启动定时器开始进行wifi连接
    if (!mico_rtos_is_timer_running(&wifi_led_timer)) mico_rtos_start_timer(&wifi_led_timer);

    IPStatusTypedef para;
    micoWlanGetIPStatus(&para, Station);
    strcpy(strMac, para.mac);

}

#define ELAND_AP_SSID       "TC1-AP"
#define ELAND_AP_KEY        "12345678"
#define ELAND_AP_LOCAL_IP   "192.168.0.1"
#define ELAND_AP_DNS_SERVER "192.168.0.1"
#define ELAND_AP_NET_MASK   "255.255.255.0"

void ap_init()
{
    os_log("Soft_ap_Server");
    network_InitTypeDef_st wNetConfig;
    memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
    strcpy((char *)wNetConfig.wifi_ssid, ELAND_AP_SSID);
    strcpy((char *)wNetConfig.wifi_key, ELAND_AP_KEY);
    wNetConfig.wifi_mode = Soft_AP;
    wNetConfig.dhcpMode = DHCP_Server;
    wNetConfig.wifi_retry_interval = 100;
    strcpy((char *)wNetConfig.local_ip_addr, ELAND_AP_LOCAL_IP);
    strcpy((char *)wNetConfig.net_mask, ELAND_AP_NET_MASK);
    strcpy((char *)wNetConfig.dnsServer_ip_addr, ELAND_AP_DNS_SERVER);
    os_log("ssid:%s  key:%s", wNetConfig.wifi_ssid, wNetConfig.wifi_key);
    micoWlanStart(&wNetConfig);
}

