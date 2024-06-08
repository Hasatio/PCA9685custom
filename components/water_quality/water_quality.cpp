#include "water_quality.h"
#include "wq_i2c.h"
#include "wq_analog.h"
#include "wq_digital.h"
#include "wq_pump.h"
#include "wq_servo.h"

namespace esphome {
namespace water_quality {

// WaterQuality water_quality_instance;
// WQ_I2C i2c(&water_quality_instance);

//     // WQ_I2C i2c;
    Analog an;
    Digital dig;
    Pump pump;
    Servo ser;

// WaterQuality::WaterQuality() { wq_i2c_ = new WQ_I2C(this); }

void EEPROM_write(int address, float value)
{
    byte *data = (byte*)&(value);
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(address + i, data[i]);
}
float EEPROM_read(int address)
{
    float value = 0.0;
    byte *data = (byte*)&(value);
    for (int i = 0; i < sizeof(value); i++)
        data[i] = EEPROM.read(address + i);
    return value;
}

void PH_Setup()
{
    float acidVoltage = 0.41;// 2.03244; // V
    float neutralVoltage = 1.78;// 1.500; // V
    float baseVoltage = 3.02;// 0.96756; // V

    float acidPh = 4.0;
    float neutralPh = 7.0;
    float basePh = 10.0;

    if (EEPROM.read(PH1ADDR) == 0xFF && EEPROM.read(PH1ADDR + 1) == 0xFF && EEPROM.read(PH1ADDR + 2) == 0xFF && EEPROM.read(PH1ADDR + 3) == 0xFF)
        EEPROM_write(PH1ADDR, neutralPh); // New EEPROM, write typical pH value
    if (EEPROM.read(Volt1ADDR) == 0xFF && EEPROM.read(Volt1ADDR + 1) == 0xFF && EEPROM.read(Volt1ADDR + 2) == 0xFF && EEPROM.read(Volt1ADDR + 3) == 0xFF)
        EEPROM_write(Volt1ADDR, neutralVoltage); // New EEPROM, write typical pH voltage
    if (EEPROM.read(PH2ADDR) == 0xFF && EEPROM.read(PH2ADDR + 1) == 0xFF && EEPROM.read(PH2ADDR + 2) == 0xFF && EEPROM.read(PH2ADDR + 3) == 0xFF)
        EEPROM_write(PH2ADDR, acidPh); // New EEPROM, write typical pH value
    if (EEPROM.read(Volt2ADDR) == 0xFF && EEPROM.read(Volt2ADDR + 1) == 0xFF && EEPROM.read(Volt2ADDR + 2) == 0xFF && EEPROM.read(Volt2ADDR + 3) == 0xFF)
        EEPROM_write(Volt2ADDR, acidVoltage); // New EEPROM, write typical pH voltage
    
    float eepromPH1 = EEPROM_read(PH1ADDR); // Load the value of the pH board from the EEPROM
    float eepromVolt1 = EEPROM_read(Volt1ADDR); // Load the voltage of the pH board from the EEPROM
    float eepromPH2 = EEPROM_read(PH2ADDR); // Load the value of the pH board from the EEPROM
    float eepromVolt2 = EEPROM_read(Volt2ADDR); // Load the voltage of the pH board from the EEPROM

    ESP_LOGD(TAG,"PH1ADDR = %d    eepromPH1 = %f", PH1ADDR, eepromPH1);
    ESP_LOGD(TAG,"Volt1ADDR = %d    eepromVolt1 = %f", Volt1ADDR, eepromVolt1);
    ESP_LOGD(TAG,"PH2ADDR = %d    eepromPH2 = %f", PH2ADDR, eepromPH2);
    ESP_LOGD(TAG,"Volt2ADDR = %d    eepromVolt2 = %f", Volt2ADDR, eepromVolt2);

    float PH_Cal[2][2] = {eepromPH1, eepromVolt1, eepromPH2, eepromVolt2};

    an.set_PH_Cal(PH_Cal);
}
void PH_Clear()
{
    for (uint8_t i = 0; i < EEPROM_SIZE; i++)
        EEPROM.write(i, 0xFF);

    EEPROM.commit();

    ESP_LOGD(TAG, "PH Clear");
    PH_Setup();
}
void EC_Setup()
{
    for (uint8_t i = 0; i < EEPROM_SIZE; i++)
        ESP_LOGD(TAG, "eeprom%d: %f", i, EEPROM_read(i));
}
void EC_Clear()
{
    for (uint8_t i = 0; i < EEPROM_SIZE; i++)
        EEPROM.write(i, 0xFF);

    EEPROM.commit();

    ESP_LOGD(TAG, "EC Clear");
    EC_Setup();
}

void WaterQuality::setup()
{
    EEPROM.begin(EEPROM_SIZE);
    ADS1115_Setup(ADS1X15_ADDRESS1);
    ADS1115_Setup(ADS1X15_ADDRESS2);
    MCP23008_Setup(MCP23008_ADDRESS);
    PCA9685_Setup(PCA9685_I2C_ADDRESS);
    PH_Setup();
    EC_Setup();
}
void WaterQuality::dump_config()
{
    EC_Setup();
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
//  PCA9685

    ESP_LOGCONFIG(TAG, "PCA9685:");
    if (this->extclk_) {
        ESP_LOGI(TAG, "  EXTCLK: enabled");
    } else {
        ESP_LOGI(TAG, "  EXTCLK: disabled");
        ESP_LOGI(TAG, "  Frequency: %.0f Hz", this->frequency_);
    }
    if (this->is_failed()) {
        ESP_LOGE(TAG, "  Setting up PCA9685 failed!");
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Pumps

    ESP_LOGCONFIG(TAG, "Pumps:");
    uint8_t dose = 0, circ = 0;
    float* calib_gain = pump.get_Pump_Calibration_Gain();
    uint8_t* type = pump.get_Pump_Type();

    for (uint8_t i = 0; i < 6; i++)
        if (type[i] == 1)
            dose += 1;
        else if (type[i] == 2)
            circ += 1;

    ESP_LOGI(TAG, "  Pump dose quantity: %d", dose);
    ESP_LOGI(TAG, "  Pump circ quantity: %d", circ);
    for (uint8_t i = 0; i < 6; i++)
    {
        ESP_LOGCONFIG(TAG, "  Pump%d:", i + 1);
        ESP_LOGI(TAG, "    Pump Calibration Gain = %.2f", calib_gain[i]);
        ESP_LOGI(TAG, "    Pump Type = %d", type[i]);
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Level

    ESP_LOGCONFIG(TAG, "Level:");
    uint16_t *resmin = an.get_ResMin(), *resmax = an.get_ResMax();
    for (uint8_t i = 0; i < sizeof(resmin) / sizeof(resmin[0]); i++)
        ESP_LOGI(TAG, "  ResMin%d = %d, ResMax%d = %d", i + 1, resmin[i], i + 1, resmax[i]);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PH

    ESP_LOGCONFIG(TAG, "PH:");
    ESP_LOGI(TAG, "  PH_ch = %d, PH_type = %d", an.get_PH_Ch(), an.get_PH_Type());

    float eepromPH1 = EEPROM_read(PH1ADDR); // Load the value of the pH board from the EEPROM
    float eepromVolt1 = EEPROM_read(Volt1ADDR); // Load the voltage of the pH board from the EEPROM
    float eepromPH2 = EEPROM_read(PH2ADDR); // Load the value of the pH board from the EEPROM
    float eepromVolt2 = EEPROM_read(Volt2ADDR); // Load the voltage of the pH board from the EEPROM

    ESP_LOGI(TAG,"  PH1ADDR = %d    eepromPH1 = %f", PH1ADDR, eepromPH1);
    ESP_LOGI(TAG,"  Volt1ADDR = %d    eepromVolt1 = %f", Volt1ADDR, eepromVolt1);
    ESP_LOGI(TAG,"  PH2ADDR = %d    eepromPH2 = %f", PH2ADDR, eepromPH2);
    ESP_LOGI(TAG,"  Volt2ADDR = %d    eepromVolt2 = %f", Volt2ADDR, eepromVolt2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  EC

    ESP_LOGCONFIG(TAG, "EC:");
    ESP_LOGI(TAG, "  EC_ch = %d, EC_type = %d", an.get_EC_Ch(), an.get_EC_Type());

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
void WaterQuality::loop()
{
    pump.Calibration_Controller();
    sensor();
}
void WaterQuality::update()
{
    float a[8], p[16] = {0}, e[6] = {0};
    bool d[4];

    ADS1115_Driver(a);
    an.Analog_Input_Driver(a);

    dig.Digital_Output_Driver(d);
    MCP23008_Driver(d);
    dig.Digital_Input_Driver(d);

    pump.Generic_Pump_Driver(p);
    ser.Servo_Driver(p);
    PCA9685_Driver(p);
}

void WaterQuality::version(const uint8_t ver)
{
    an.set_version(ver);
}
void WaterQuality::pump_calibration_mode(std::vector<bool> &pcal)
{
    bool* pcal_ = pump.get_Pump_Calibration_Mode();
    std::vector<bool> pc(pcal_, pcal_ + 6);

    if (pc != pcal)
    {
        for (uint8_t i = 0; i < 6; i++)
        {
            pcal_[i] = pcal[i];
            ESP_LOGD(TAG, "Pump_Calibration_Mode[%d] = %d", i, pcal_[i]);
        }
    }
}
void WaterQuality::pump_calibration_gain(const std::vector<float> &pcal)
{
    float pcal_[6];

    for (uint8_t i = 0; i < 6; i++)
        pcal_[i] = pcal[i];

    pump.set_Pump_Calibration_Gain(pcal_);
}
void WaterQuality::pump_type(const std::vector<uint8_t> &ptype)
{
    uint8_t ptype_[6];
    
    for (uint8_t i = 0; i < 6; i++)
        ptype_[i] = ptype[i];

    pump.set_Pump_Type(ptype_);   
}
void WaterQuality::pump_mode(std::vector<uint8_t> &pmode)
{
    uint8_t* pmode_ = pump.get_Pump_Mode();
    std::vector<uint8_t> pm(pmode_, pmode_ + 6);

    if (pm != pmode)
    {
        for (uint8_t i = 0; i < 6; i++)
        {
            pmode_[i] = pmode[i];
            ESP_LOGD(TAG, "Pump_Mode[%d] = %d", i, pmode_[i]);
        }
    }
}
void WaterQuality::pump_dose(std::vector<float> &pdose)
{
    uint8_t* ptype = pump.get_Pump_Type();
    uint8_t* pmode = pump.get_Pump_Mode();
    uint8_t* pstat = pump.get_Pump_Status();
    float* pdose_ = pump.get_Pump_Dose();
    std::vector<float> pd(pdose_, pdose_ + 6);

    if (pd != pdose && !pump.get_Pump_Calibration_Mode_Check())
        for (uint8_t i = 0; i < 6; i++)
            if (ptype[i] == 1)
                if (pmode[i] == 0)
                {
                    if (!(pstat[i] == 1))
                    {
                        pdose_[i] = pdose[i];
                        ESP_LOGD(TAG, "Pump_Dose[%d] = %f", i, pdose_[i]);
                    }
                    // else
                    // {
                    //     pdose_[i] += pdose[i];
                    //     ESP_LOGD(TAG, "Pump_Dose[%d] = %f", i, pdose_[i]);
                    // }
                }
}
void WaterQuality::pump_circulation(std::vector<float> &pcirc)
{
    uint8_t* ptype = pump.get_Pump_Type();
    uint8_t* pmode = pump.get_Pump_Mode();
    uint8_t* pstat = pump.get_Pump_Status();
    float* pcirc_ = pump.get_Pump_Circulation();
    std::vector<float> pc(pcirc_, pcirc_ + 6);

    if (pc != pcirc && !pump.get_Pump_Calibration_Mode_Check())
        for (uint8_t i = 0; i < 6; i++)
            if (ptype[i] == 2)
                if (pmode[i] == 0)
                {
                    if (!(pstat[i] == 1))
                    {
                        pcirc_[i] = pcirc[i];
                        ESP_LOGD(TAG, "Pump_Circulation[%d] = %f", i, pcirc_[i]);
                    }
                    // else
                    // {
                    //     pcirc_[i] += pcirc[i];
                    //     ESP_LOGD(TAG, "Pump_Circulation[%d] = %f", i, pcirc_[i]);
                    // }
                }
}
void WaterQuality::pump_reset(std::vector<bool> &pres)
{
    bool* pres_ = pump.get_Pump_Reset();
    std::vector<bool> pr(pres_, pres_ + 6);

    if (pr != pres && !pump.get_Pump_Calibration_Mode_Check())
    {
        for (uint8_t i = 0; i < 6; i++)
        {
            pres_[i] = pres[i];
            ESP_LOGD(TAG, "Pump_Reset[%d] = %d", i, pres_[i]);
        }
    }
}
void WaterQuality::servo_mode(std::vector<bool> &smode)
{
    bool* smode_ = ser.get_Servo_Mode();
    std::vector<bool> sm(smode_, smode_ + 8);
    
    if (sm != smode)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            smode_[i] = smode[i];
            ESP_LOGD(TAG, "Servo_Mode[%d] = %d", i, smode_[i]);
        }
    }
}
void WaterQuality::servo_position(std::vector<uint8_t> &spos)
{
    uint8_t* spos_ = ser.get_Servo_Position();
    std::vector<uint8_t> sp(spos_, spos_ + 8);
    
    if (sp != spos)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            spos_[i] = spos[i];
            ESP_LOGD(TAG, "Servo_Position[%d] = %d", i, spos_[i]);
        }
    }
}
void WaterQuality::level_res(const std::vector<uint16_t> &rmin, const std::vector<uint16_t> &rmax)
{    
    uint16_t rminArray[2] = {0}, rmaxArray[2] = {0};

    for (uint8_t i = 0; i < rmin.size(); i++)
    {
        rminArray[i] = rmin[i];
        rmaxArray[i] = rmax[i];
    }

    an.set_ResMin(rminArray);
    an.set_ResMax(rmaxArray);
}
void WaterQuality::ph_calibration(float ph)
{
    if (ph > 0)
    {
        float voltage = an.phVoltage;
        float eepromPH1 = 7, eepromVolt1, eepromPH2 = 4, eepromVolt2;
        static float eepromPH1_backup = eepromPH1, eepromPH2_backup = eepromPH2;
        
        eepromPH1 = EEPROM_read(PH1ADDR); // Load the value of the pH board from the EEPROM
        eepromVolt1 = EEPROM_read(Volt1ADDR); // Load the voltage of the pH board from the EEPROM
        eepromPH2 = EEPROM_read(PH2ADDR); // Load the value of the pH board from the EEPROM
        eepromVolt2 = EEPROM_read(Volt2ADDR); // Load the voltage of the pH board from the EEPROM
    
        static bool q = 0;
        if (round(ph) != round(eepromPH2) && round(ph) != round(eepromPH2_backup) && !q || round(ph) == round(eepromPH1))
        {
            q = 1;
            eepromPH1 = ph;
            eepromVolt1 = voltage;
            EEPROM_write(PH1ADDR, ph); // Store the current pH value as
            EEPROM_write(Volt1ADDR, voltage); // Store the current pH voltage as
            ESP_LOGI(TAG,"Calibrated to pH = %f", ph);
        }
        else if (round(ph) != round(eepromPH1) && round(ph) != round(eepromPH1_backup) && q || round(ph) == round(eepromPH2))
        {
            q = 0;
            eepromPH2 = ph;
            eepromVolt2 = voltage;
            EEPROM_write(PH2ADDR, ph); // Store the current pH value as
            EEPROM_write(Volt2ADDR, voltage); // Store the current pH voltage as
            ESP_LOGI(TAG,"Calibrated to pH = %f", ph);
        }
        
        EEPROM.commit();

        ESP_LOGD(TAG,"PH1ADDR = %d    eepromPH1 = %f", PH1ADDR, eepromPH1);
        ESP_LOGD(TAG,"Volt1ADDR = %d    eepromVolt1 = %f", Volt1ADDR, eepromVolt1);
        ESP_LOGD(TAG,"PH2ADDR = %d    eepromPH2 = %f", PH2ADDR, eepromPH2);
        ESP_LOGD(TAG,"Volt2ADDR = %d    eepromVolt2 = %f", Volt2ADDR, eepromVolt2);

        float PH_Cal[2][2] = {eepromPH1, eepromVolt1, eepromPH2, eepromVolt2};

        an.set_PH_Cal(PH_Cal);
    }
    else
        PH_Clear();
}
void WaterQuality::ph(const uint8_t ch, const uint8_t type)
{
    an.set_PH_Ch(ch);
    an.set_PH_Type(type);
}
void WaterQuality::ec_calibration(float cal)
{
    an.set_EC_Cal(cal);
}
void WaterQuality::ec(const uint8_t ch, const uint8_t type)
{
    an.set_EC_Ch(ch);
    an.set_EC_Type(type);
}
void WaterQuality::digital_out(std::vector<bool> &dout)
{
    bool* dout_ = dig.get_Digital_Output();
    std::vector<bool> d(dout_, dout_ + 4);

    if (d != dout)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            dout_[i] = dout[i];
            ESP_LOGD(TAG, "DigOut_Status[%d] = %d", i, dout_[i]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor
void WaterQuality::sensor()
{
    if (this->Pump_Tot_ != nullptr)
    {
        uint32_t (*ptot)[2] = pump.get_Pump_Total();
        std::stringstream pt;

        for (uint8_t i = 0; i < 6; i++)
            if (i == 0)
                pt << std::fixed << std::setprecision(4) << ptot[i][0] + ptot[i][1] / 10000000.0;
            else
                pt << "," << std::fixed << std::setprecision(4) << ptot[i][0] + ptot[i][1] / 10000000.0;
            
        this->Pump_Tot_->publish_state(pt.str());
    }
    if (this->Pump_Stat_ != nullptr)
    { 
        uint8_t* pstat = pump.get_Pump_Status();
        std::stringstream ps;

        for (uint8_t i = 0; i < 6; i++)
            if (i == 0)
                ps << std::fixed << std::setprecision(0) << static_cast<int>(pstat[i]);
            else
                ps << "," << std::fixed << std::setprecision(0) << static_cast<int>(pstat[i]);
            
        this->Pump_Stat_->publish_state(ps.str());
    }
    if (this->Servo_Stat_ != nullptr)
    { 
        bool* sstat = ser.get_Servo_Status();
        std::stringstream ss;

        for (uint8_t i = 0; i < 8; i++)
            if (i == 0)
                ss << std::fixed << std::setprecision(0) << static_cast<int>(sstat[i]);
            else
                ss << "," << std::fixed << std::setprecision(0) << static_cast<int>(sstat[i]);
            
        this->Servo_Stat_->publish_state(ss.str());
    }
    if (this->AnInWTemp_Val_ != nullptr)    { this->AnInWTemp_Val_->publish_state(an.get_WatTemp_Val()); }
    if (this->AnInVPow_Val_ != nullptr)     { this->AnInVPow_Val_->publish_state(an.get_VoltPow_Val()); }
    if (this->AnInLvl_Perc_ != nullptr) 
    {
        float* lvl = an.get_Lvl_Perc();
        std::stringstream ap;

        for (uint8_t i = 0; i < 2; i++)
            if (i == 0)
                ap << std::fixed << std::setprecision(2) << lvl[i];
            else
                ap << "," << std::fixed << std::setprecision(2) << lvl[i];

        this->AnInLvl_Perc_->publish_state(ap.str());
    }
    if (this->AnInEC_Val_ != nullptr)       { this->AnInEC_Val_->publish_state(an.get_EC_Val()); }
    if (this->AnInPH_Val_ != nullptr)       { this->AnInPH_Val_->publish_state(an.get_PH_Val()); }
    if (this->AnInGen_Val_ != nullptr) 
    {
        float* gen = an.get_Gen_Val();
        std::stringstream av;

        for (uint8_t i = 0; i < 2; i++)
            if (i == 0)
                av << std::fixed << std::setprecision(2) << gen[i];
            else
                av << "," << std::fixed << std::setprecision(2) << gen[i];
    
        this->AnInGen_Val_->publish_state(av.str());
    }
    if (this->DigIn_Stat_ != nullptr) 
    {
        bool* din = dig.get_Digital_Input();
        std::stringstream ds;

        for (uint8_t i = 0; i < 4; i++)
            if (i == 0)
                ds << std::fixed << std::setprecision(0) << static_cast<int>(din[i]);
            else
                ds << "," << std::fixed << std::setprecision(0) << static_cast<int>(din[i]);

        this->DigIn_Stat_->publish_state(ds.str());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace water_quality
}  // namespace esphome