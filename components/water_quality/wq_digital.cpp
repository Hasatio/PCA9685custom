#include "water_quality.h"
#include "wq_i2c.h"
#include "wq_digital.h"

namespace esphome {
namespace water_quality {

void Digital::Digital_Input_Driver(bool inputs[])
{
    bool digital[4];

    for (size_t i = 0; i < 4; i++)
    {
    if (inputs[i] == 0)
        DigIn_FilterCoeff[i]--;
    else
        DigIn_FilterCoeff[i]++;

    if (DigIn_FilterCoeff[i] == 0)
        digital[i] = 0;
    else if (DigIn_FilterCoeff[i] == 4)
        digital[i] = 1;
    }
  
    set_Digital_In(digital);
}
void Digital::Digital_Output_Driver(bool outputs[])
{
    bool* digital = get_Digital_Out();

    for (size_t i = 0; i < 4; i++)
        outputs[i] = digital[i];
}
}  // namespace water_quality
}  // namespace esphome