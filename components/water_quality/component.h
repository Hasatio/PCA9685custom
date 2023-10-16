#pragma once

#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace water_quality {

class MyComponent : public Component 
{
public:

float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; }

uint16_t AnIn_TempRes = 1000; //temperature sensor model pt1000 and its resistance is 1k
float AnOut_Vcc, AnOut_Temp, TempRes;
uint8_t DigIn_FilterCoeff[4][10];

void setup() override;
void loop() override;

void calibration(const std::vector<uint8_t> &cx1, const std::vector<uint8_t> &cy1, const std::vector<uint8_t> &cx2, const std::vector<uint8_t> &cy2, const std::vector<uint8_t> &cx3, const std::vector<uint8_t> &cy3, const std::vector<uint8_t> &cx4, const std::vector<uint8_t> &cy4) 
{ 
    this->Pump_Calib_X1 = cx1;
    this->Pump_Calib_Y1 = cy1;
    this->Pump_Calib_X2 = cx2;
    this->Pump_Calib_Y2 = cy2;
    this->Pump_Calib_X3 = cx3;
    this->Pump_Calib_Y3 = cy3;
    this->Pump_Calib_X4 = cx4;
    this->Pump_Calib_Y4 = cy4;
}

void dose(double d)
{

}

void AnOut_Status(sensor::Sensor *a) 
{
    AnOut_Status_ = a;
}

private:

std::vector<uint8_t> Pump_Calib_X1{};
std::vector<uint8_t> Pump_Calib_Y1{};
std::vector<uint8_t> Pump_Calib_X2{};
std::vector<uint8_t> Pump_Calib_Y2{};
std::vector<uint8_t> Pump_Calib_X3{};
std::vector<uint8_t> Pump_Calib_Y3{};
std::vector<uint8_t> Pump_Calib_X4{};
std::vector<uint8_t> Pump_Calib_Y4{};
std::vector<uint8_t> Pump_Mode{};
std::vector<uint8_t> Pump_Dose{};
std::vector<uint8_t> Pump_Circulation{};
std::vector<uint8_t> Pump_Reset{};
std::vector<uint8_t> Servo_Mode{};
std::vector<uint8_t> Servo_Position{};
std::vector<uint8_t> AnIn_LvlResMin{};
std::vector<uint8_t> AnIn_LvlResMax{};
std::vector<uint8_t> DigIn_Status{};

sensor::Sensor *Pump_TimeConstant_{nullptr};
sensor::Sensor *Pump_Total_{nullptr};
sensor::Sensor *Pump_Status_{nullptr};
sensor::Sensor *Servo_Status_{nullptr};
sensor::Sensor *AnIn_Status_{nullptr};
sensor::Sensor *AnIn_SensPerc_{nullptr};
sensor::Sensor *DigOut_Status_{nullptr};

};

// template<typename... Ts> class BrakeAction : public Action<Ts...> {
//  public:
//   explicit BrakeAction(HBridgeFan *parent) : parent_(parent) {}

//   void play(Ts... x) override { this->parent_->brake(); }

//   HBridgeFan *parent_;
// };

template<typename... Ts> class EzoPMPDoseVolumeAction : public Action<Ts...> {
 public:
  EzoPMPDoseVolumeAction(MyComponent *ezopmp) : ezopmp_(ezopmp) {}

  void play(Ts... x) override { this->ezopmp_->dose(this->volume_.value(x...)); }
  TEMPLATABLE_VALUE(double, volume)

 protected:
  EzoPMP *ezopmp_;
};

}  // namespace water_quality
}  // namespace esphome
