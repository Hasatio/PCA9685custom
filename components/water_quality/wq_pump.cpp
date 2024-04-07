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
void IRAM_ATTR Pump::Timer(void* arg)
{
    Pump* pumpInstance = static_cast<Pump*>(arg);
    float* pump = pumpInstance->get_Pump_Time();
    pumpInstance->Dosing_Controller(pump);
    pumpInstance->Circulation_Controller(pump);
}
void Pump::Calibration_Controller()
{
    uint8_t* calib_cond = get_Pump_Calibration_Condition();
    bool* calib_mode = get_Pump_Calibration_Mode();
    float* calib_vol = get_Pump_Calibration_Volume();
    float* calib_gain = get_Pump_Calibration_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* model = get_Pump_Model();
    uint8_t* mode = get_Pump_Mode();
    float* dose = get_Pump_Dose();
    float* circ = get_Pump_Circulation();
    uint8_t calib_time = 120;
    uint8_t calib_ml = 200;

    for (size_t i = 0; i < 6; i++)
    {
        // if (type[i] > 0 && model[i] > 0)
        //     if (calib_mode[i])
        //     {
        //         if (calib_con[i] == 0)
        //         {
        //             calib_vol[i] = calib_time;
        //             set_Calibration_Condition(1);
        //             ESP_LOGI(TAG, "Pump%d Calibration Start", i + 1);
        //             ESP_LOGI(TAG, "Calibration_Condition = %d", calib_con[i]);
        //         }
        //         else if (calib_con[i] == 1 && calib_vol[i] == 0)
        //         {
        //             set_Calibration_Condition(2);
        //             ESP_LOGI(TAG, "Pump%d Calibration Finish", i + 1);
        //             ESP_LOGI(TAG, "Calibration_Condition = %d", calib_con[i]);
        //         }
        //     }
        //     else
        //     {
            //     if (calib_con[i] == 1 && calib_vol[i] > 0)
            //     {
            //         calib_vol[i] = 0;
            //         set_Calibration_Condition(0);
            //         ESP_LOGI(TAG, "Pump%d Calibration Abort", i + 1);
            //         ESP_LOGI(TAG, "Calibration_Condition = %d", calib_con[i]);
            //     }
                // if (calib_con[i] == 2)
                // {
                //     set_Calibration_Condition(0);
                //     ESP_LOGI(TAG, "Calibration_Condition = %d", calib_con[i]);
                // }
            // }
        
        if (type[i] > 0 && model[i] > 0)
            switch (calib_cond[i])
            {
                case 0:
                    if (calib_mode[i] && !get_Pump_Calibration_Mode_Check())
                    {
                        if (model[i] == 1)
                            calib_vol[i] = calib_time;
                        else if (model[i] == 2)
                            calib_vol[i] = calib_ml;

                        calib_cond[i] = 1;
                        set_Pump_Calibration_Mode_Check(1);
                        ESP_LOGI(TAG, "Pump%d Calibration Start", i + 1);
                        ESP_LOGI(TAG, "Calibration_Condition = %d", calib_cond[i]);
                    }
                    break;

                case 1:
                    if (calib_mode[i] && calib_vol[i] == 0)
                    {
                        calib_cond[i] = 2;
                        set_Pump_Calibration_Mode_Check(0);
                        ESP_LOGI(TAG, "Pump%d Calibration Finish", i + 1);
                        ESP_LOGI(TAG, "Calibration_Condition = %d", calib_cond[i]);
                    }
                    else if (!calib_mode[i] && calib_vol[i] > 0)
                    {
                        calib_vol[i] = 0;
                        calib_cond[i] = 3;
                        ESP_LOGI(TAG, "Pump%d Calibration Abort", i + 1);
                        ESP_LOGI(TAG, "Calibration_Condition = %d", calib_cond[i]);
                    }
                    break;

                case 2:
                    if (!calib_mode[i])
                    {
                        calib_cond[i] = 0;
                        ESP_LOGI(TAG, "Calibration_Condition = %d", calib_cond[i]);
                    }
                    break;

                case 3:
                    calib_cond[i] = 0;
                    set_Pump_Calibration_Mode_Check(0);
                    break;
                
                default:
                    break;
            }
    }
}

void Pump::Generic_Pump_Driver(float pwm[])
{
    uint32_t (*tot)[2] = get_Pump_Total();
    bool* reset = get_Pump_Reset();
    float* pump = get_Pump_Time();
    float min = 0, min_[6] = {0};

    std::copy(pump, pump + 6, min_);
    std::sort(min_, min_ + 6);

    for (size_t i = 0; i < 6; ++i)
    {
        if (min_[i] > 0)
        {
            min = min_[i];
            break;
        }
        else
            min = 0;
    }
    set_Min_Time(min);
    ESP_LOGD(TAG, "min = %f", get_Min_Time());
    ESP_LOGD(TAG, "min2 = %f", min);

    if (get_Min_Time() != min){
        Timer_Setup(min);ESP_LOGD(TAG, "here");}
    else if (min == 0)
    {
        Dosing_Controller(pump);
        Circulation_Controller(pump);
    }

    for (size_t i = 0; i < 6; i++)
    {
        if (pump[i] > 0)
            pwm[i] = 1;
        else if (pump[i] < 0)
            pwm[i] = 0;
        else
        {
            if (pwm[i])
                ESP_LOGD(TAG, "Pump_Total[%d] = %d.%04d", i, tot[i][0], tot[i][1]);

            pwm[i] = 0;
        }

        if (reset[i])
        {
            tot[i][0] = 0;
            tot[i][1] = 0;
        }
    }
}
void Pump::Dosing_Controller(float pump[])
{
    bool* calib_mode = get_Pump_Calibration_Mode();
    float* calib_vol = get_Pump_Calibration_Volume();
    float* calib_gain = get_Pump_Calibration_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* model = get_Pump_Model();
    uint8_t* mode = get_Pump_Mode();
    uint8_t* stat = get_Pump_Status();
    float* dose = get_Pump_Dose();
    uint32_t (*tot)[2] = get_Pump_Total();
    float min = get_Min_Time();

    for (size_t i = 0; i < 6; i++)
    {
        if (type[i] == 1 && model[i] == 1)
            if (get_Pump_Calibration_Mode_Check())
            {
                if (pump[i] > 0 && stat[i] == 4)
                    calib_vol[i]--;
                ESP_LOGD(TAG, "calib_vol[%d] = %f", i, calib_vol[i]);

                if (calib_vol[i] > 0)
                {
                    pump[i] = 1;
                    stat[i] = 4;
                }
                else
                {
                    pump[i] = 0;
                    stat[i] = 0;
                }
            }
            else
            {
                if (pump[i] > 0)
                {
                    if (dose[i] > 0)
                    {
                        tot[i][1] = static_cast<uint32_t>(tot[i][1] + calib_gain[i] * min * 10000);
                        tot[i][0] += static_cast<uint32_t>(tot[i][1] / 10000000);
                        if (tot[i][1] >= 10000000)
                            tot[i][1] = 0;
                    }

                    ESP_LOGD(TAG, "dose[%d] = %f", i, dose[i]);
                    ESP_LOGD(TAG, "min = %f", min);

                    dose[i] = fabs(dose[i] - min * calib_gain[i]);
                    if (dose[i] < 0)
                        dose[i] = 0;
                }

                switch (mode[i])
                {
                    case 0:
                        if (dose[i] > 0)
                            if (!(i % 2 == 0 || (i % 2 == 1 && stat[i - 1] != 1)))
                                stat[i] = 0;
                        else if (dose[i] == 0)
                            stat[i] = 2;
                        break;

                    case 1:
                        if (dose[i] > 0)
                            if (i % 2 == 0 || (i % 2 == 1 && stat[i - 1] != 1))
                                stat[i] = 1;
                            else
                                stat[i] = 0;
                        else if (dose[i] == 0)
                            stat[i] = 2;
                        break;

                    case 2:
                        stat[i] = 3;
                        dose[i] = 0;
                        break;
                    
                    default:
                        break;
                }

                if (stat[i] == 1)
                    if (dose[i] > calib_gain[i])
                        pump[i] = 1;
                    else
                        pump[i] = dose[i] / calib_gain[i];
                else
                    pump[i] = 0;
            }
    }
}
void Pump::Circulation_Controller(float pump[])
{
    bool* calib_mode = get_Pump_Calibration_Mode();
    float* calib_vol = get_Pump_Calibration_Volume();
    float* calib_gain = get_Pump_Calibration_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* model = get_Pump_Model();
    uint8_t* mode = get_Pump_Mode();
    uint8_t* stat = get_Pump_Status();
    float* circ = get_Pump_Circulation();
    uint32_t (*tot)[2] = get_Pump_Total();
    float min = get_Min_Time();

    for (size_t i = 0; i < 6; i++)
    {
        if (type[i] == 2 && model[i] == 1)
            if (get_Pump_Calibration_Mode_Check())
            {
                if (pump[i] > 0 && stat[i] == 4)
                    calib_vol[i]--;
                
                if (calib_vol[i] > 0)
                {
                    pump[i] = 1;
                    stat[i] = 4;
                }
                else
                {
                    pump[i] = 0;
                    stat[i] = 0;
                }
            }
            else
            {
                if (pump[i] > 0)
                {
                    if (circ[i] > 0)
                    {
                        tot[i][1] = static_cast<uint32_t>(tot[i][1] + calib_gain[i] * min * 10000);
                        tot[i][0] += static_cast<uint32_t>(tot[i][1] / 10000000);
                        if (tot[i][1] >= 10000000)
                            tot[i][1] = 0;
                    }

                    circ[i] = fabs(circ[i] - min * calib_gain[i]);
                    if (circ[i] < 0)
                        circ[i] = 0;
                }

                switch (mode[i])
                {
                    case 0:
                        if (circ[i] > 0)
                            if (!(i % 2 == 0 || (i % 2 == 1 && stat[i - 1] != 1)))
                                stat[i] = 0;
                        else if (circ[i] == 0)
                            stat[i] = 2;
                        break;

                    case 1:
                        if (circ[i] > 0)
                            if (i % 2 == 0 || (i % 2 == 1 && stat[i - 1] != 1))
                                stat[i] = 1;
                            else
                                stat[i] = 0;
                        else if (circ[i] == 0)
                            stat[i] = 2;
                        break;

                    case 2:
                        stat[i] = 3;
                        circ[i] = 0;
                        break;
                    
                    default:
                        break;
                }

                if (stat[i] == 1)
                    if (circ[i] > calib_gain[i])
                        pump[i] = 1;
                    else
                        pump[i] = circ[i] / calib_gain[i];
                else
                    pump[i] = 0;
            }
    }
}

void Pump::Serial_Com_Pump_Driver(float pump[])
{
    float* calib_gain = get_Pump_Calibration_Gain();
    uint8_t* type = get_Pump_Type();
    uint8_t* model = get_Pump_Model();
    uint8_t* mode = get_Pump_Mode();
    uint8_t* stat = get_Pump_Status();
    float* dose = get_Pump_Dose();
    uint32_t (*tot)[2] = get_Pump_Total();
    double vol = 0;

    for (size_t i = 0; i < 6; i++)
    {
        if (type[i] == 1 && model[i] == 2)
        {
            // if (pump[i] > 0)
            // {
            //         if (dose[i] > 0)
            //         {
            //             tot[i][1] = static_cast<uint32_t>(tot[i][1] + calib_gain[i] * min * 10000);
            //             tot[i][0] += static_cast<uint32_t>(tot[i][1] / 10000000);
            //             if (tot[i][1] >= 10000000)
            //                 tot[i][1] = 0;
            //         }
            // }

            switch (mode[i])
            {
                case 0:
                    if (dose[i] == 0)
                        stat[i] = 2;
                    break;

                case 1:
                    if (dose[i] > 0)
                        stat[i] = 1;
                    else if (dose[i] == 0)
                        stat[i] = 2;
                    break;

                case 2:
                    stat[i] = 3;
                    break;
                
                default:
                    break;
            }

            if (stat[i] == 1)
            {
                pump[i] = dose[i];
                dose[i] = 0;
                ESP_LOGI(TAG,"pump[%d] = %f", i, pump[i]);
            }
            else if(stat[i] == 3)
                pump[i] = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace water_quality
}  // namespace esphome