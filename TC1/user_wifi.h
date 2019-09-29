#ifndef __USER_WIFI_H_
#define __USER_WIFI_H_

#include "mico.h"
#include "mico_wlan.h"
#include "micokit_ext.h"

enum
{
   WIFI_STATE_FAIL,
   WIFI_STATE_NOCONNECT,
   WIFI_STATE_CONNECTING,
   WIFI_STATE_CONNECTED,
};

/*
typedef struct _WifiResult
{
    char ssid[32];
    wlan_sec_type_t security;
}
WifiResult;
*/

#define WIFI_SCAN_RESULT_JSON "{'success':%d,'ssids':[%s],'secs':[%s]}"

extern char* wifi_ret;

extern char wifi_status;
extern bool scaned;
//extern WifiResult* wifi_rets;

extern void wifi_init(void);
extern void ap_init(void);
extern void wifi_connect(char* wifi_ssid, char* wifi_key);

#endif
