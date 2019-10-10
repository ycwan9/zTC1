#define os_log(format, ...)  custom_log("OTA", format, ##__VA_ARGS__)
#include "TimeUtils.h"

#include "mico.h"
#include "main.h"
#include "user_udp.h"
#include "user_mqtt_client.h"
#include "user_function.h"

mico_timer_t power_timer;

static uint32_t clock_count_last = 0;
static uint32_t clock_count = 0;
static uint32_t timer_count = 0;
static uint32_t timer_irq_count = 0;

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

