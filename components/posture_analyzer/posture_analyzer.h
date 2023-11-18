#pragma once

#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include <Adafruit_ADS1X15.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_MAX1704X.h>
#include <BluetoothSerial.h>

  // i2c ayarları
  #define SDA 21 
  #define SCL 22
  #define frq 800000
  /*
  #define SDA_1 32
  #define SCL_1 33
  #define freq_1 800000
  
  #define SDA_2 25
  #define SCL_2 26
  #define freq_2 800000
  
  #define SDA_3 27
  #define SCL_3 14
  #define freq_3 800000
  
  #define SDA_4 9
  #define SCL_4 10
  #define freq_4 800000
  */

  // i2c adres ayarları
  #define ADS1X15_ADDRESS1 0x48
  #define ADS1X15_ADDRESS2 0x49
  #define ADS1X15_ADDRESS3 0x4a
  #define ADS1X15_ADDRESS4 0x4b
  #define ADXL345_ADDRESS 0x53
  #define MAX17048_ADDRESS 0x36

extern "C" { uint8_t temprature_sens_read(); }

static const char *const TAG = "mysensor";

namespace esphome {
namespace posture_analyzer {

  // TwoWire I2C_1 = TwoWire(0);
  // TwoWire I2C_2 = TwoWire(1);
  
  BluetoothSerial SerialBT; // bluetooth yeni adlandırması
  
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth off--Run `make menuconfig` to enable it 
  #endif

  //Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
  Adafruit_ADS1115 ads1; // ads1115 yeni adlandırması
  Adafruit_ADS1115 ads2;
  Adafruit_ADS1115 ads3;
  Adafruit_ADS1115 ads4;
  //Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */
  
  Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345); // adx345 ayarı
  
  Adafruit_MAX17048 maxlipo; // max17048 yeni adlandırması
  
class MyComponent : public PollingComponent // ana sınıf
{
public:
float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; } // çalışma önceliği



void internal_temp();
void bt_set();
void bt();
void i2c_set();
void ads1115_set();
void ads1115();
void adxl345_set();
void adxl345();
void max17048_set();
void max17048();
void sensor();

void setup() override;
void loop() override; // döngü fonksiyonu
void dump_config() override
{
    ESP_LOGI("data", "Bluetooth is ready to pair\nDevice name: %s",btname);
}
void update() override;

void bluetooth(String b);
void gain(float g);
void sample(sensor::Sensor *sample);
void sample_sec(sensor::Sensor *sample_sec);

protected:
String btname = "ESP32"; // bt standart adı
uint16_t adc[16];
uint64_t sayac = 0;
float volts[16], x, y, z, voltage, percentage, mygain = 1.0, temperature = NAN;
double adxlmultiplier;
String data = "";
bool success = false;

sensor::Sensor *sample_{nullptr}; // sensör değişkeni
sensor::Sensor *sample_sec_{nullptr}; // sensör değişkeni

}; // class MyComponent

} //namespace posture_analyzer
} //namespace esphome