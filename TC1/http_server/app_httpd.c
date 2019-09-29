/**
 ******************************************************************************
 * @file    app_https.c
 * @author  QQ DING
 * @version V1.0.0
 * @date    1-September-2015
 * @brief   The main HTTPD server initialization and wsgi handle.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include <httpd.h>
#include <http_parse.h>
#include <http-strings.h>

#include "mico.h"
#include "httpd_priv.h"
#include "app_httpd.h"
#include "user_gpio.h"
#include "user_wifi.h"
#include "main.h"
#include "web_data.c"

static bool is_http_init;
static bool is_handlers_registered;
struct httpd_wsgi_call g_app_handlers[];

static int http_get_index_page(httpd_request_t *req)
{
    OSStatus err = kNoErr;

    err = httpd_send_all_header(req, HTTP_RES_200, sizeof(index_html), HTTP_CONTENT_HTML_ZIP);
    require_noerr_action(err, exit, app_httpd_log("ERROR: Unable to send http wifisetting headers."));

    err = httpd_send_body(req->sock, index_html, sizeof(index_html));
    require_noerr_action(err, exit, app_httpd_log("ERROR: Unable to send http wifisetting body."));

exit:
    return err;
}

static int http_get_tc1_status(httpd_request_t *req)
{
    const unsigned char* sockets = get_socket_status();
    char* ap_name = "TC1-AP";
    char* ap_pwd = "12345678";
    char* ip = "192.168.33.222";
    char* mask = "255.255.255.0";
    char* gateway = "192.168.33.1";
    char* tc1_status = malloc(256);
    sprintf(tc1_status, TC1_STATUS_JSON, sockets, (int)sys_config->micoSystemConfig.reserved,
        sys_config->micoSystemConfig.ssid, sys_config->micoSystemConfig.user_key,
        ap_name, ap_pwd, ip, mask, gateway);

    OSStatus err = kNoErr;
    send_http(tc1_status, strlen(tc1_status), exit, &err);

exit:
    return err;
}

static int http_set_socket_status(httpd_request_t *req)
{
    OSStatus err = kNoErr;

    int buf_size = 512;
    char *buf = malloc(buf_size);

    err = httpd_get_data(req, buf, buf_size);
    require_noerr(err, exit);

    set_socket_status(buf);

    send_http("OK", 2, exit, &err);

exit:
    if (buf) free(buf);
    return err;
}

static int http_get_wifi_config(httpd_request_t *req)
{
    OSStatus err = kNoErr;

    const unsigned char* status = get_socket_status();

    err = httpd_send_all_header(req, HTTP_RES_200, strlen(socket_status), HTTP_CONTENT_HTML_STR);
    require_noerr_action(err, exit, app_httpd_log("ERROR: Unable to send http socket_status headers."));

    err = httpd_send_body(req->sock, status, strlen(socket_status));
    require_noerr_action(err, exit, app_httpd_log("ERROR: Unable to send http socket_status body."));

exit:
    return err;
}

static int http_set_wifi_config(httpd_request_t *req)
{
    OSStatus err = kNoErr;

    int buf_size = 512;
    int ssid_size = 32;
    int key_size = 64;
    char *buf = malloc(buf_size);
    char *wifi_ssid = malloc(ssid_size);
    char *wifi_key = malloc(key_size);

    err = httpd_get_data(req, buf, buf_size);
    require_noerr(err, exit);

    err = httpd_get_tag_from_post_data(buf, "ssid", wifi_ssid, ssid_size);
    require_noerr(err, exit);

    err = httpd_get_tag_from_post_data(buf, "key", wifi_key, key_size);
    require_noerr(err, exit);

    wifi_connect(wifi_ssid, wifi_key);

    send_http("OK", 2, exit, &err);

exit:
    if (buf) free(buf);
    if (wifi_ssid) free(wifi_ssid);
    if (wifi_key) free(wifi_key);
    return err;
}

static int http_get_wifi_scan(httpd_request_t *req)
{
    OSStatus err = kNoErr;
    if (scaned)
    {
        scaned = false;
        send_http(wifi_ret, strlen(wifi_ret), exit, &err);
        free(wifi_ret);
    }
    else
    {
        send_http("NO", 2, exit, &err);
    }

exit:
    return err;
}

static int http_set_wifi_scan(httpd_request_t *req)
{
    micoWlanStartScanAdv();
    OSStatus err = kNoErr;
    send_http("OK", 2, exit, &err);
exit:
    return err;
}

struct httpd_wsgi_call g_app_handlers[] = {
    {"/", HTTPD_HDR_DEFORT, 0, http_get_index_page, NULL, NULL, NULL},
    {"/socket", HTTPD_HDR_DEFORT, 0, NULL, http_set_socket_status, NULL, NULL},
    {"/status", HTTPD_HDR_DEFORT, 0, http_get_tc1_status, NULL, NULL, NULL},
    {"/wifi/config", HTTPD_HDR_DEFORT, 0, http_get_wifi_config, http_set_wifi_config, NULL, NULL},
    {"/wifi/scan", HTTPD_HDR_DEFORT, 0, http_get_wifi_scan, http_set_wifi_scan, NULL, NULL},
};

static int g_app_handlers_no = sizeof(g_app_handlers)/sizeof(struct httpd_wsgi_call);

static void app_http_register_handlers()
{
    int rc;
    rc = httpd_register_wsgi_handlers(g_app_handlers, g_app_handlers_no);
    if (rc) {
        app_httpd_log("failed to register test web handler");
    }
}

static int _app_httpd_start()
{
    OSStatus err = kNoErr;
    app_httpd_log("initializing web-services");

    /*Initialize HTTPD*/
    if(is_http_init == false) {
        err = httpd_init();
        require_noerr_action(err, exit, app_httpd_log("failed to initialize httpd"));
        is_http_init = true;
    }

    /*Start http thread*/
    err = httpd_start();
    if(err != kNoErr) {
        app_httpd_log("failed to start httpd thread");
        httpd_shutdown();
    }
exit:
    return err;
}

int app_httpd_start(void)
{
    OSStatus err = kNoErr;

    err = _app_httpd_start();
    require_noerr(err, exit);

    if (is_handlers_registered == false) {
        app_http_register_handlers();
        is_handlers_registered = true;
    }

exit:
    return err;
}

int app_httpd_stop()
{
    OSStatus err = kNoErr;

    /* HTTPD and services */
    app_httpd_log("stopping down httpd");
    err = httpd_stop();
    require_noerr_action(err, exit, app_httpd_log("failed to halt httpd"));

exit:
    return err;
}
