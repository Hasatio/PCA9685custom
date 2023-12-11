#pragma once

#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H

#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/automation.h"
#include <array>
#include <vector>
#include <iterator>
#include <sstream>
#include <iomanip>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

namespace esphome {
namespace water_quality {

static const char *const TAG = "mycomponent";

enum ADS1115Registers
{
    ADS1115_REGISTER_CONVERSION = 0x00,
    ADS1115_REGISTER_CONFIG = 0x01,
};
enum ADS1115_Multiplexer
{
  ADS1115_MULTIPLEXER_P0_N1 = 0b000,
  ADS1115_MULTIPLEXER_P0_N3 = 0b001,
  ADS1115_MULTIPLEXER_P1_N3 = 0b010,
  ADS1115_MULTIPLEXER_P2_N3 = 0b011,
  ADS1115_MULTIPLEXER_P0_NG = 0b100,
  ADS1115_MULTIPLEXER_P1_NG = 0b101,
  ADS1115_MULTIPLEXER_P2_NG = 0b110,
  ADS1115_MULTIPLEXER_P3_NG = 0b111,
};
enum ADS1115_Gain
{
  ADS1115_GAIN_6P144 = 0b000,
  ADS1115_GAIN_4P096 = 0b001,
  ADS1115_GAIN_2P048 = 0b010,
  ADS1115_GAIN_1P024 = 0b011,
  ADS1115_GAIN_0P512 = 0b100,
  ADS1115_GAIN_0P256 = 0b101,
};
enum ADS1115_DataRate
{
  ADS1115_DATA_RATE_8_SPS = 0b000,
  ADS1115_DATA_RATE_16_SPS = 0b001,
  ADS1115_DATA_RATE_32_SPS = 0b010,
  ADS1115_DATA_RATE_64_SPS = 0b011,
  ADS1115_DATA_RATE_128_SPS = 0b100,
  ADS1115_DATA_RATE_250_SPS = 0b101,
  ADS1115_DATA_RATE_475_SPS = 0b110,
  ADS1115_DATA_RATE_860_SPS = 0b111,
};
enum ADS1115_Resolution
{
  ADS1115_16_BITS = 16,
  ADS1015_12_BITS = 12,
};

enum MCP23008_PinMode
{
  FLAG_NONE,
  FLAG_INPUT,
  FLAG_OUTPUT,
  FLAG_OPEN_DRAIN,
  FLAG_PULLUP,
  FLAG_PULLDOWN,
};
enum MCP23008_Registers 
{
  MCP23008_IODIR = 0x00,
  MCP23008_IPOL = 0x01,
  MCP23008_GPINTEN = 0x02,
  MCP23008_DEFVAL = 0x03,
  MCP23008_INTCON = 0x04,
  MCP23008_IOCON = 0x05,
  MCP23008_GPPU = 0x06,
  MCP23008_INTF = 0x07,
  MCP23008_INTCAP = 0x08,
  MCP23008_GPIO = 0x09,
  MCP23008_OLAT = 0x0A,
};
enum MCP23008_InterruptMode : uint8_t { NO_INTERRUPT = 0, CHANGE, RISING, FALLING };


class WaterQuality : public PollingComponent, public i2c::I2CDevice//, public sensor::Sensor, public Analog, public Digital, public Pump, public Servo
{
public:
float get_setup_priority() const override { return esphome::setup_priority::DATA; }

uint16_t PwmFreq = 1000;

    // Adafruit_PWMServoDriver pwm;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PCA9685
void pca9685_set()
{
    // // tcaselect(0);
    // Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS, Wire);
    
    // if (!pwm.begin()) 
    // {
    //   ESP_LOGE(TAG,"Failed to initialize PCA9685.");
    // //   while (1);
    // }
    // /*
    // * In theory the internal oscillator (clock) is 25MHz but it really isn't
    // * that precise. You can 'calibrate' this by tweaking this number until
    // * you get the PWM update frequency you're expecting!
    // * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
    // * is used for calculating things like writeMicroseconds()
    // * Analog servos run at ~50 Hz updates, It is importaint to use an
    // * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
    // * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
    // *    the I2C PCA9685 chip you are setting the value for.
    // * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
    // *    expected value (50Hz for most ESCs)
    // * Setting the value here is specific to each individual I2C PCA9685 chip and
    // * affects the calculations for the PWM update frequency. 
    // * Failure to correctly set the int.osc value will cause unexpected PWM results
    // */
    // pwm.setOscillatorFrequency(27000000);
    // pwm.setPWMFreq(PwmFreq);
}
void pca9685()
{
    // tcaselect(0);
    // for (uint8_t pin=0; pin<16; pin++) 
    // {
    // pwm.setPWM(pin, 4096, 0);       // turns pin fully on
    // delay(100);
    // pwm.setPWM(pin, 0, 4096);       // turns pin fully off
    // }
    // for (uint16_t i=0; i<4096; i += 8) 
    // {
    //     for (uint8_t pwmnum=0; pwmnum < 16; pwmnum++) 
    //     {
    //     pwm.setPWM(pwmnum, 0, (i + (4096/16)*pwmnum) % 4096 );
    //     }
    // }
    // for (uint16_t i=0; i<4096; i += 8) 
    // {
    //     for (uint8_t pwmnum=0; pwmnum < 16; pwmnum++) 
    //     {
    //     pwm.setPin(pwmnum, (i + (4096/16)*pwmnum) % 4096 );
    //     }
        
    //     ESP_LOGD(TAG,"pwm = %d", i);
    // }

    // for (uint8_t i = 0; i < Pump_Total[1].size(); i++) 
    //     {
    //         Pump_Total[1][i] += i;
    //     }
}

void ADS1115_Setup(uint8_t address);
void ADS1115_Driver(float analog_voltage[]);
float ADS1115_Read(ADS1115Multiplexer multi);

void set_multiplexer(ADS1115Multiplexer multiplexer) { multiplexer_ = multiplexer; }
void set_gain(ADS1115Gain gain) { gain_ = gain; }
void set_continuous_mode(bool continuous_mode) { continuous_mode_ = continuous_mode; }
void set_data_rate(ADS1115DataRate data_rate) { data_rate_ = data_rate; }
void set_resolution(ADS1115Resolution resolution) { resolution_ = resolution; }

uint8_t get_multiplexer() const { return multiplexer_; }
uint8_t get_gain() const { return gain_; }
uint8_t get_continuous_mode() const { return continuous_mode_; }
uint8_t get_data_rate() const { return data_rate_; }
uint8_t get_resolution() const { return resolution_; }

void MCP23008_Setup(uint8_t address);
void MCP23008_Driver(float digital[]);

bool MCP23008_digital_read(uint8_t pin);
void MCP23008_digital_write(uint8_t pin, bool value);
void MCP23008_pin_mode(uint8_t pin, MCP23008_PinMode mode);
void MCP23008_pin_interrupt_mode(uint8_t pin, MCP23008_InterruptMode interrupt_mode);

void set_open_drain_ints(const bool value) { this->open_drain_ints_ = value; }
std::string dump_summary() const override;

void set_pin(uint8_t pin) { pin_ = pin; }
void set_inverted(bool inverted) { inverted_ = inverted; }
void set_flags(gpio::Flags flags) { flags_ = flags; }
void set_interrupt_mode(MCP23008_InterruptMode interrupt_mode) { interrupt_mode_ = interrupt_mode; }


void sensor();

void setup() override;
void dump_config() override;
void loop() override;
void update() override;

void version(const uint8_t v);
void pump_calib_gain(const std::vector<float> &pcg);
void pump_type(const std::vector<uint8_t> &ptype, const uint8_t d, const uint8_t c);
void pump_dose(std::vector<uint16_t> &pdose);
void pump_circulation(std::vector<uint16_t> &pcirc);
void pump_mode(std::vector<uint8_t> &pmode);
void pump_reset(std::vector<bool> &pres);
void servo_mode(std::vector<bool> &smode);
void servo_position(std::vector<uint8_t> &spos);
void level_res(const std::vector<uint16_t> &rmin, const std::vector<uint16_t> &rmax);
void ec(const uint8_t ch, const uint8_t type);
void ph(const uint8_t ch, const uint8_t type);
void digital_out(std::vector<bool> &dout);

// protected:
// std::vector<ADS1115Sensor *> sensors_;

// class MySensor : public PollingComponent, public sensor::Sensor
// {
// public:
// MySensor(WaterQuality *parent) : parent_(parent) {}

// void update() override;

void Pump_Tot                   (text_sensor::TextSensor *ptot)     { Pump_Tot_ = ptot; }
void Pump_Stat                  (text_sensor::TextSensor *pstat)    { Pump_Stat_ = pstat; }
void Servo_Stat                 (text_sensor::TextSensor *servo)    { Servo_Stat_ = servo; }
void WaterTemp_Sensor_Driver    (sensor::Sensor *wtemp)             { AnInWT_Val_ = wtemp; }
void VPow_Sensor_Driver         (sensor::Sensor *vpow)              { AnInVPow_Val_ = vpow; }
void AnLevel_Sensor_Driver      (text_sensor::TextSensor *level)    { AnInLvl_Perc_ = level; }
void WaterEC_Sensor_Driver      (sensor::Sensor *ec)                { AnInEC_Val_ = ec; }
void WaterPH_Sensor_Driver      (sensor::Sensor *ph)                { AnInPH_Val_ = ph; }
void AnGen_Input_Driver         (text_sensor::TextSensor *a)        { AnInGen_Val_ = a; }
void DigIn_Stat                 (text_sensor::TextSensor *din)      { DigIn_Stat_ = din; }

protected:
text_sensor::TextSensor *Pump_Tot_{nullptr};
text_sensor::TextSensor *Pump_Stat_{nullptr};
text_sensor::TextSensor *Servo_Stat_{nullptr};
sensor::Sensor *AnInWT_Val_{nullptr};
sensor::Sensor *AnInVPow_Val_{nullptr};
text_sensor::TextSensor *AnInLvl_Perc_{nullptr};
sensor::Sensor *AnInEC_Val_{nullptr};
sensor::Sensor *AnInPH_Val_{nullptr};
text_sensor::TextSensor *AnInGen_Val_{nullptr};
text_sensor::TextSensor *DigIn_Stat_{nullptr};

uint16_t prev_config_{0};
ADS1115Multiplexer multiplexer_;
ADS1115Gain gain_;
bool continuous_mode_;
ADS1115DataRate data_rate_;
ADS1115Resolution resolution_;

bool MCP23008_read_reg(uint8_t reg, uint8_t *value);
bool MCP23008_write_reg(uint8_t reg, uint8_t value);
void MCP23008_update_reg(uint8_t pin, bool pin_value, uint8_t reg_a);
uint8_t olat_{0x00};
uint8_t pin_;
bool inverted_;
gpio::Flags flags_;
MCP23008_InterruptMode interrupt_mode_;
bool open_drain_ints_;
};

template<typename... Ts> class PumpModeAction : public Action<Ts...> {
    public:
    PumpModeAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<uint8_t> data = this->val_.value(x...);

    this->parent_->pump_mode(data);
    }

    TEMPLATABLE_VALUE(std::vector<uint8_t>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class PumpDoseAction : public Action<Ts...> {
    public:
    PumpDoseAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<uint16_t> data = this->val_.value(x...);

    this->parent_->pump_dose(data);
    }

    TEMPLATABLE_VALUE(std::vector<uint16_t>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class PumpCirculationAction : public Action<Ts...> {
    public:
    PumpCirculationAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<uint16_t> data = this->val_.value(x...);

    this->parent_->pump_circulation(data);
    }

    TEMPLATABLE_VALUE(std::vector<uint16_t>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class PumpResetAction : public Action<Ts...> {
    public:
    PumpResetAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<bool> data = this->val_.value(x...);

    this->parent_->pump_reset(data);
    }

    TEMPLATABLE_VALUE(std::vector<bool>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class ServoModeAction : public Action<Ts...> {
    public:
    ServoModeAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<bool> data = this->val_.value(x...);

    this->parent_->servo_mode(data);
    }

    TEMPLATABLE_VALUE(std::vector<bool>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class ServoPositionAction : public Action<Ts...> {
    public:
    ServoPositionAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<uint8_t> data = this->val_.value(x...);

    this->parent_->servo_position(data);
    }

    TEMPLATABLE_VALUE(std::vector<uint8_t>, val);

    protected:
    WaterQuality *parent_;
};
template<typename... Ts> class DigitalOutAction : public Action<Ts...> {
    public:
    DigitalOutAction(WaterQuality *parent) : parent_(parent){};
    
    void play(Ts... x) 
    {
    std::vector<bool> data = this->val_.value(x...);

    this->parent_->digital_out(data);
    }

    TEMPLATABLE_VALUE(std::vector<bool>, val);

    protected:
    WaterQuality *parent_;
};

}  // namespace water_quality
}  // namespace esphome

#endif  // WATER_QUALITY_H