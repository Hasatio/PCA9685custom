#include "mux.h"
#include "analog.h"
#include "water_quality.h"

namespace esphome {
namespace water_quality {

    Mux mux;
    Analog an;
    MyComponent comp;

void Analog::ads1115_set()
{ 
    // tcaselect(0);
    if (!ads1.begin(ADS1X15_ADDRESS1))
    {
      ESP_LOGE(analog,"Failed to initialize ADS1115_1.");
    //   while (1);
    }
    if (!ads2.begin(ADS1X15_ADDRESS2))
    {
      ESP_LOGE(analog,"Failed to initialize ADS1115_2.");
    //   while (1);
    }

    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    
    //                                          ADS1015          ADS1115
    //                                          -------          -------
    // GAIN_TWOTHIRDS  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // GAIN_ONE        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    // GAIN_TWO        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    // GAIN_FOUR       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // GAIN_EIGHT      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // GAIN_SIXTEEN    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
    ads1.setGain(GAIN_TWOTHIRDS);
    ads2.setGain(GAIN_TWOTHIRDS);
    
    // RATE_ADS1115_8SPS (0x0000)   ///< 8 samples per second
    // RATE_ADS1115_16SPS (0x0020)  ///< 16 samples per second
    // RATE_ADS1115_32SPS (0x0040)  ///< 32 samples per second
    // RATE_ADS1115_64SPS (0x0060)  ///< 64 samples per second
    // RATE_ADS1115_128SPS (0x0080) ///< 128 samples per second (default)
    // RATE_ADS1115_250SPS (0x00A0) ///< 250 samples per second
    // RATE_ADS1115_475SPS (0x00C0) ///< 475 samples per second
    // RATE_ADS1115_860SPS (0x00E0) ///< 860 samples per second
    ads1.setDataRate(RATE_ADS1115_860SPS);
    ads2.setDataRate(RATE_ADS1115_860SPS);
    
    // ADS1X15_REG_CONFIG_MUX_DIFF_0_1 (0x0000) ///< Differential P = AIN0, N = AIN1 (default)
    // ADS1X15_REG_CONFIG_MUX_DIFF_0_3 (0x1000) ///< Differential P = AIN0, N = AIN3
    // ADS1X15_REG_CONFIG_MUX_DIFF_1_3 (0x2000) ///< Differential P = AIN1, N = AIN3
    // ADS1X15_REG_CONFIG_MUX_DIFF_2_3 (0x3000) ///< Differential P = AIN2, N = AIN3
    // ADS1X15_REG_CONFIG_MUX_SINGLE_0 (0x4000) ///< Single-ended AIN0
    // ADS1X15_REG_CONFIG_MUX_SINGLE_1 (0x5000) ///< Single-ended AIN1
    // ADS1X15_REG_CONFIG_MUX_SINGLE_2 (0x6000) ///< Single-ended AIN2
    // ADS1X15_REG_CONFIG_MUX_SINGLE_3 (0x7000) ///< Single-ended AIN3
    // ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1);

    ec.begin();
    ph.begin();
}
void Analog::ads1115()
{
    // tcaselect(0);
    for(size_t i = 0; i < 4; i++)
    {
        adc[i] = ads1.readADC_SingleEnded(i%4);
        volts[i] = ads1.computeVolts(adc[i]);
        // ESP_LOGD(analog,"ads%d = %f", i+1, volts[i]);
    }
    for(size_t i = 4; i < 8; i++){
        adc[i] = ads2.readADC_SingleEnded(i%4);
        volts[i] = ads2.computeVolts(adc[i]);
        // ESP_LOGD(analog,"ads%d = %f", i+1, volts[i]);
    }
}


void MyComponent::level_res(const std::vector<uint16_t> &rmin, const std::vector<uint16_t> &rmax)
{
    this->an.AnInLvl_ResMin = rmin;
    this->an.AnInLvl_ResMax = rmax;
}
void MyComponent::ec(const uint8_t ch, const uint8_t type)
{
    an.AnInEC_Ch = ch;
    an.AnInEC_Type = type;
}
void MyComponent::ph(const uint8_t ch, const uint8_t type)
{
    an.AnInPH_Ch = ch;
    an.AnInPH_Type = type;
}

void Analog::Analog_Input_Driver()
{
    tot = AnInEC_Ch + AnInEC_Ch;
    rnd = round((6 - tot) / 2);
    AnInGen_Ch[0] = 6 - tot - rnd - 1;
    AnInGen_Ch[1] = 6 - tot - AnInGen_Ch[0];
    AnInGen_Ch[0] = (AnInGen_Ch[0] == AnInEC_Ch)? AnInGen_Ch[0] - 1 : AnInGen_Ch[0];
    AnInGen_Ch[1] = (AnInGen_Ch[1] == AnInEC_Ch)? AnInGen_Ch[1] + 1 : AnInGen_Ch[1];

        ESP_LOGD(analog,"ads = %f", volts[3+4]);
        ESP_LOGD(analog,"ads = %f", (ads2.readADC_SingleEnded(3)/10));
    VPow = (float)volts[1] * 6; //Vin = Vout * (R1 + R2) / R2; R1 = 10k, R2 = 2k
    LvlPerc[0] = (float)volts[2] * 100 / 5 * AnInLvl_ResMax[0] / (1000 + AnInLvl_ResMax[0]) - 5 * AnInLvl_ResMin[0] / (1000 + AnInLvl_ResMin[0]); //Vout = Vin * R2 / (R1 + R2); R1 = 10k
    LvlPerc[1] = (float)volts[3] * 100 / 5 * AnInLvl_ResMax[1] / (1000 + AnInLvl_ResMax[1]) - 5 * AnInLvl_ResMin[1] / (1000 + AnInLvl_ResMin[1]); //Vout = Vin * R2 / (R1 + R2); R1 = 10k

    ecValue = volts[AnInEC_Ch];
    phValue = volts[AnInPH_Ch];
    AnGen[0] = volts[AnInGen_Ch[0] + 4];
    AnGen[1] = volts[AnInGen_Ch[1] + 4];
}

void Analog::ec_ph()
{
	unsigned long now = millis();
	if (now - last[0] >= intervals[0]) //1000ms interval
	{
		last[0] = now;
		if (calibrationIsRunning)
		{
			Serial.println(F("[main]...>>>>>> calibration is running, to exit send exitph or exitec through serial <<<<<<"));
			//water temperature
			temperature = getWaterTemperature();
			//EC
			ecVoltage = ads2.readADC_SingleEnded(3) / 10;
			Serial.print(F("[EC Voltage]... ecVoltage: "));
			Serial.println(ecVoltage);
			ecValue = ec.readEC(ecVoltage, temperature); // convert voltage to EC with temperature compensation
			Serial.print(F("[EC Read]... EC: "));
			Serial.print(ecValue);
			Serial.println(F("ms/cm"));
			//pH
			phVoltage = ads2.readADC_SingleEnded(1) / 10;
			Serial.print(F("[pH Voltage]... phVoltage: "));
			Serial.println(phVoltage);
			phValue = ph.readPH(phVoltage, temperature);
			Serial.print(F("[pH Read]... pH: "));
			Serial.println(phValue);
		}

		if (readSerial(cmd))
		{
			strupr(cmd);
			if (calibrationIsRunning || strstr(cmd, "PH") || strstr(cmd, "EC"))
			{
				calibrationIsRunning = true;
				Serial.println(F("[]... >>>>>calibration is now running PH and EC are both reading, if you want to stop this process enter EXITPH or EXITEC in Serial Monitor<<<<<"));
				if (strstr(cmd, "PH"))
				{
					ph.calibration(phVoltage, temperature, cmd); //PH calibration process by Serail CMD
				}
				if (strstr(cmd, "EC"))
				{
					ec.calibration(ecVoltage, temperature, cmd); //EC calibration process by Serail CMD
				}
			}
			if (strstr(cmd, "EXITPH") || strstr(cmd, "EXITEC"))
			{
				calibrationIsRunning = false;
			}
		}
	}
	if (now - last[3] >= intervals[3]) //5000ms interval
	{
		last[3] = now; 
		if (!calibrationIsRunning)
		{
			temperature = getWaterTemperature(); // read your temperature sensor to execute temperature compensation
			Serial.print("temperature:");
			Serial.print(temperature, 1);
			Serial.println("^C");

			ecVoltage = ads2.readADC_SingleEnded(3) / 10;
			Serial.print("ecVoltage:");
			Serial.println(ecVoltage, 4);

			ecValue = ec.readEC(ecVoltage, temperature); // convert voltage to EC with temperature compensation
			Serial.print("EC:");
			Serial.print(ecValue, 4);
			Serial.println("ms/cm");

			phVoltage = ads2.readADC_SingleEnded(1) / 10; // read the voltage
			Serial.print("phVoltage:");
			Serial.println(phVoltage, 4);
			phValue = ph.readPH(phVoltage, temperature); // convert voltage to pH with temperature compensation
			Serial.print("pH:");
			Serial.println(phValue, 4);
		}
	}
}

void Analog::ec_ph2()
{
    if(millis()-timepoint>1000U)                             //time interval: 1s
    {
        timepoint = millis();
        //temperature = readTemperature();                   // read your temperature sensor to execute temperature compensation
        // voltagePH = analogRead(PH_PIN)/1024.0*5000;          // read the ph voltage
        // phValue    = ph.readPH(voltagePH,WT);       // convert voltage to pH with temperature compensation
        Serial.print("pH:");
        Serial.print(phValue,2);
        // voltageEC = analogRead(EC_PIN)/1024.0*5000;
        // ecValue    = ec.readEC(voltageEC,WT);       // convert voltage to EC with temperature compensation
        Serial.print(", EC:");
        Serial.print(ecValue,2);
        Serial.println("ms/cm");
    }
    if(readSerial(cmd)){
        strupr(cmd);
        if(strstr(cmd,"PH")){
            ph.calibration(voltagePH,WT,cmd);       //PH calibration process by Serail CMD
        }
        if(strstr(cmd,"EC")){
            ec.calibration(voltageEC,WT,cmd);       //EC calibration process by Serail CMD
        }
    }
}


}  // namespace water_quality
}  // namespace esphome