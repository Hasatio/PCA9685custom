#include "component.h"

namespace esphome {
namespace water_quality {

static const char *TAG = "mysensor";



void MyComponent::setup() 
{
    Pump_Calib_X1.resize(8);
    Pump_Calib_Y1.resize(8);
}

void MyComponent::loop() 
{
    ESP_LOGD(TAG,"%d", sizeof(Pump_Calib_X1));
    ESP_LOGD(TAG,"%d", sizeof(Pump_Calib_X1[0]));

    for (size_t i = 0; i < sizeof(Pump_Calib_X2) / sizeof(Pump_Calib_X2[0]); i++)
    {
        // ESP_LOGD(TAG,"x1[%d] = %d", i, Pump_Calib_X1[i]);
        // ESP_LOGD(TAG,"y1[%d] = %d", i, Pump_Calib_Y1[i]);
        ESP_LOGD(TAG,"x2[%d] = %d", i, Pump_Calib_X2[i]);
        // ESP_LOGD(TAG,"y2[%d] = %d", i, Pump_Calib_Y2[i]);
        // ESP_LOGD(TAG,"x3[%d] = %d", i, Pump_Calib_X3[i]);
        // ESP_LOGD(TAG,"y3[%d] = %d", i, Pump_Calib_Y3[i]);
        // ESP_LOGD(TAG,"x4[%d] = %d", i, Pump_Calib_X4[i]);
        // ESP_LOGD(TAG,"y4[%d] = %d", i, Pump_Calib_Y4[i]);
    }

    // for (size_t i = 0; i < sizeof(Pump_Type) / sizeof(Pump_Type[0]); i++)
    // {
    //     ESP_LOGD(TAG,"%d = %d", i, Pump_Type[i]);
    // }
        // ESP_LOGD(TAG,"data[0] = %d", Pump_Dose[0]);
    
        // ESP_LOGD(TAG,"%d", dd);

delay(1000);
}

}  // namespace water_quality
}  // namespace esphome