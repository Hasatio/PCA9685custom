#include "mcs.h"
#include "mcs_i2c.h"
#include "mcs_digital.h"

namespace esphome {
namespace mcs {

    MCS_Digital dig;

void EEPROM_Write(int address, float value)
{
    byte *data = (byte*)&(value);
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(address + i, data[i]);
}
float EEPROM_Read(int address)
{
    float value = 0.0;
    byte *data = (byte*)&(value);
    for (int i = 0; i < sizeof(value); i++)
        data[i] = EEPROM.read(address + i);
    return value;
}
void EEPROM_Setup()
{
    uint8_t digital = EEPROM_Read(LED_L_ADDR); // Load the value of the digital from the EEPROM
    bool digital_status[] = {0};
    for (uint8_t i = 0; i < 20; i++)
    {
        if (digital > i)
            digital_status[i] = 1;
        else break;
    }
    dig.set_Digital_Output(digital_status);
}

void MCS::setup()
{
    EEPROM.begin(MCS_EEPROM_SIZE);
    EEPROM_Setup();
    MCP23017_Setup(MCP23017_ADDRESS1);
    MCP23017_Setup(MCP23017_ADDRESS2);
}
void MCS::dump_config()
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  I2C

    ESP_LOGCONFIG(TAG, "");
    ESP_LOGCONFIG(TAG, "I2C:");
    
    LOG_I2C_DEVICE(this);
    LOG_UPDATE_INTERVAL(this);
    if (this->is_failed())
        ESP_LOGE(TAG, "  Communication failed!");
    else
        ESP_LOGI(TAG, "  Communication Successfulled!");
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TCA9548

    Wire.beginTransmission(TCA9548_ADDRESS);
    if(!Wire.endTransmission())
    {
        ESP_LOGCONFIG(TAG, "  TCA9548:");

        for (uint8_t t=0; t<8; t++)
        {
            tcaselect(t);
            ESP_LOGI(TAG, "    Channel %d:", t);

            for (uint8_t addr = 0; addr<=127; addr++) 
            {
                if (addr == TCA9548_ADDRESS) continue;

                Wire.beginTransmission(addr);
                if (!Wire.endTransmission()) 
                {
                    if (addr < 16)
                        ESP_LOGI(TAG, "      Found I2C 0x0%x",addr);
                    else
                        ESP_LOGI(TAG, "      Found I2C 0x%x",addr);
                }
                else if(Wire.endTransmission() == 4)
                    ESP_LOGE(TAG, "      Found the same I2C 0x%x",addr);
            }
        }
    }
    else
    {
        ESP_LOGCONFIG(TAG, "  I2C Devices:");

        for (uint8_t addr = 0; addr<=127; addr++) 
        {
            Wire.beginTransmission(addr);
            if (!Wire.endTransmission()) 
            {
                if (addr < 16)
                    ESP_LOGI(TAG, "    Found I2C 0x0%x",addr);
                else
                    ESP_LOGI(TAG, "    Found I2C 0x%x",addr);
            }
            else if(Wire.endTransmission() == 4)
                ESP_LOGE(TAG, "    Found the same I2C 0x%x",addr);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ESP_LOGCONFIG(TAG, "Digital:");

uint8_t digital = EEPROM_Read(LED_L_ADDR);; // Load the value of the digital from the EEPROM
ESP_LOGI(TAG, "  Value: %d", digital);

}

void MCS::loop()
{
}
void MCS::update()
{
    bool d[20] = {1};

    dig.Digital_Output_Driver(d);
    MCP23017_Driver(d);
}

void MCS::version(const uint8_t ver)
{
}
void MCS::digital_out(std::vector<bool> &dout)
{
    bool* dout_ = dig.get_Digital_Output();
    std::vector<bool> d(dout_, dout_ + 20);
    uint8_t digital = 0;

    if (d != dout)
    {
        for (uint8_t i = 0; i < 20; i++)
        {
            dout_[i] = dout[i];
            if (dout[i]) digital++;
            ESP_LOGD(TAG, "DigOut_Status[%d] = %d", i, dout_[i]);
        }
        ESP_LOGD(TAG, "digital = %d", digital);
        EEPROM_Write(LED_L_ADDR, digital); // Store the current value
        EEPROM.commit();
    }
}

}  // namespace mcs
}  // namespace esphome