#include "wq_pump.h"

namespace esphome {
namespace water_quality {

void Pump::Timer_Setup(float period)
{
    float* pump = get_Pump_Time();
    if (timer)
    {
            esp_timer_stop(timer);
            esp_timer_delete(timer);
    }

    // Timer'ı başlat
    esp_timer_create_args_t timer_args = {
        .callback = &Pump::Timer,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = nullptr,
    };
    esp_timer_create(&timer_args, &timer);
        
    esp_timer_start_periodic(timer, static_cast<uint32_t>(period * 1000000));

}

static uint32_t multFactor = 0;
static uint32_t timers = 0;
void IRAM_ATTR Pump::Timer(void* arg)
{
    timers = millis();
    Pump* pumpInstance = static_cast<Pump*>(arg);
    float* pump = pumpInstance->get_Pump_Time();
    pumpInstance->Dosing_Controller(pump);
    pumpInstance->Circulation_Controller(pump);

    // ESP_LOGI(TAG, "timer = %d", timers - multFactor);
    multFactor = timers;
}

void Pump::Pump_driver(float pwm[])
{     
    uint8_t* stat = get_Pump_Status();
    uint16_t (*tot)[2] = get_Pump_Total();
    float* pump = get_Pump_Time();
    uint8_t stat_[6] = {0};
    float min = get_Min();
    float min_, mint[6];

    for (size_t i = 0; i < 6; i++)
    {
        if (stat_[i] != stat[i])
        {
            stat_[i] = stat[i];
        }
    }

    std::copy(pump, pump + 6, mint);
    std::sort(mint, mint + 6);

    for (size_t i = 0; i < 6; ++i) 
    {
        if (mint[i] > 0)
        {
            min_ = mint[i];
            break;
        }
        else
            min_ = 0;
    }
    set_Min(min_);

    if (min != min_)
    {
        Timer_Setup(min_);
    }
    else if (min_ == 0)
    {
        Dosing_Controller(pump);
        Circulation_Controller(pump);
    }
    
    for (size_t i = 0; i < 6; i++)
    {
        if (pump[i] > 0)
            pwm[i] = 1;
        else
        {
            if (pwm[i])
                ESP_LOGD(TAG, "Pump_Total[%d] = %d.%03d", i, tot[i][0], tot[i][1]);

            pwm[i] = 0;
        }
    }
}
void Pump::Dosing_Controller(float pump[])
{
    float* calib = get_Pump_Calib_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* mode = get_Pump_Mode();
    uint8_t* stat = get_Pump_Status();
    float* dose = get_Pump_Dose();
    uint16_t (*tot)[2] = get_Pump_Total();
    bool* reset = get_Pump_Reset();
    float min = get_Min();

    for (size_t i = 0; i < 6; i++)
    {
        if (type[i] == 1)
        {
            if (pump[i] > 0)
            {
                tot[i][0] += static_cast<uint16_t>(tot[i][1] + (dose[i] > 0 ? calib[i] : 0) * min * 10) / 10000;
                tot[i][1] = static_cast<uint16_t>(tot[i][1] + (dose[i] > 0 ? calib[i] : 0) * min * 10) % 10000;
                
                dose[i] -= (pump[i] > min ? min : pump[i]) * calib[i];
            }

            if (reset[i])
            {
                tot[i][0] = 0;
                tot[i][1] = 0;
            }

            switch (mode[i])
            {
            case 0:
                pump[i] = 0;
                if (stat[i] <= 1)
                    stat[i] = 0;
                break;
            case 1:
                if (dose[i] > 0)
                    if (i % 2 == 0 || (i % 2 == 1 && mode[i - 1] == 0))
                    {
                        pump[i] = dose[i] > calib[i] ? 1 : static_cast<float>(dose[i]) / calib[i];
                        stat[i] = 1;
                    }
                    else
                    {
                        pump[i] = 0;
                        stat[i] = 0;
                    }
                else
                {
                    pump[i] = 0;
                    stat[i] = 2;
                    mode[i] = 0;
                }
                break;
            case 2:
                pump[i] = 0;
                stat[i] = 3;
                break;
            
            default:
                break;
            }
        }
    }
}
void Pump::Circulation_Controller(float pump[])
{
    float* calib = get_Pump_Calib_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* mode = get_Pump_Mode();
    uint8_t* stat = get_Pump_Status();
    float* circ = get_Pump_Circulation();
    uint16_t (*tot)[2] = get_Pump_Total();
    bool* reset = get_Pump_Reset();
    float min = get_Min();

    for (size_t i = 0; i < 6; i++)
    {
        if (type[i] == 2)
        {
            if (pump[i] > 0)
            {
                tot[i][0] += static_cast<uint16_t>(tot[i][1] + (circ[i] > 0 ? calib[i] : 0) * min * 10) / 10000;
                tot[i][1] = static_cast<uint16_t>(tot[i][1] + (circ[i] > 0 ? calib[i] : 0) * min * 10) % 10000;
                
                circ[i] -= (pump[i] > min ? min : pump[i]) * calib[i];   
            }

            if (reset[i])
            {
                tot[i][0] = 0;
                tot[i][1] = 0;
            }

            switch (mode[i])
            {
            case 0:
                pump[i] = 0;
                if (stat[i] <= 1)
                    stat[i] = 0;
                break;
            case 1:
                if (circ[i] > 0)
                    if (i % 2 == 0 || (i % 2 == 1 && mode[i - 1] == 0))
                    {
                        pump[i] = circ[i] > calib[i] ? 1 : static_cast<float>(circ[i]) / calib[i];
                        stat[i] = 1;
                    }
                    else
                    {
                        pump[i] = 0;
                        stat[i] = 0;
                    }
                else
                {
                    pump[i] = 0;
                    stat[i] = 2;
                    mode[i] = 0;
                }
                break;
            case 2:
                pump[i] = 0;
                stat[i] = 3;
                break;
            
            default:
                break;
            }
        }
    }
}

}  // namespace water_quality
}  // namespace esphome