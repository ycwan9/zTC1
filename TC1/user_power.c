#define os_log(format, ...)  custom_log("OTA", format, ##__VA_ARGS__)
#include "TimeUtils.h"

#include "mico.h"
#include "main.h"
#include "user_udp.h"
#include "user_mqtt_client.h"
#include "user_function.h"
#include "user_power.h"

mico_timer_t power_timer;

PowerRecord power_record = { 44, {
    0,1,2,3,4,5,6,7,8,9,
    10,11,12,13,14,15,16,17,18,19,
    20,21,22,23,24,25,26,27,28,29,
    30,31,32,33,34,35,36,37,38,39,
    40,41,42,43,44,45,46,47,48,49,
    50,51,52,53,54,55,56,57,58,59,
    60,61,62,63,64,65,66,67,68,69,
    70,71,72,73,74,75,76,77,78,79,
    80,81,82,83,84,85,86,87,88,89,
    90,91,92,93,94,95,96,97,98,99 } };

static uint32_t clock_count_last = 0;
static uint32_t clock_count = 0;
static uint32_t timer_count = 0;
static uint32_t timer_irq_count = 0;

char power_record_str[1101] = { 0 };

void SetPowerRecord(PowerRecord* pr, uint32_t pw)
{
    pr->powers[(++pr->idx)% PW_NUM] = pw;
}

char* GetPowerRecord(int idx)
{
    if (idx > power_record.idx) return "";

    int i = idx > 0 ? idx : (power_record.idx - PW_NUM - 1);
    char* tmp = power_record_str;
    for (; i <= power_record.idx; i++)
    {
        sprintf(tmp, "%u,", (unsigned int)power_record.powers[i%PW_NUM]);
        tmp += strlen(tmp);
    }
    *(--tmp) = 0;
    return power_record_str;
}

static void PowerTimerHandler(void* arg)
{
    uint32_t timer = 0;

    if (timer_irq_count > 1)
    {
        timer = (clock_count - clock_count_last);

        if (timer_count > 3)
        {
            timer /= 1000;
            timer += 4294967; //0xffffffff/1000;
        }
        else if (clock_count < clock_count_last)
        {
            timer += 0xffffffff;
            timer /= 1000;
        }
        else
        {
            timer /= 1000;
        }
        power = 17100000 * (timer_irq_count - 1) / timer;
        timer_count = 0;
        timer_irq_count = 0;
    }
    else
    {
        timer_count++;
    }
}

static void PowerIrqHandler(void* arg)
{
    clock_count = mico_nanosecond_clock_value();
    if (timer_irq_count == 0) clock_count_last = clock_count;
    timer_irq_count++;
}

void PowerInit(void)
{
    os_log("user_power_init");

    MicoGpioInitialize(POWER, INPUT_PULL_UP);
    mico_rtos_init_timer(&power_timer, 1000, PowerTimerHandler, NULL);
    mico_rtos_start_timer(&power_timer);

    MicoGpioEnableIRQ(POWER, IRQ_TRIGGER_FALLING_EDGE, PowerIrqHandler, NULL);
}

