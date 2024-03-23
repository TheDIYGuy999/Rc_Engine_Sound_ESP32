//
// =======================================================================================================
// ARRAYS FOR ENGINE RPM
// =======================================================================================================
//

#include <Arduino.h>

// Automatic transmission gear ratios (times 10!, for example 15 means 1.5) -----------------------
// see: https://www.allisontransmission.com/docs/default-source/marketing-materials/sa7943en_-2017-vocational-model-guide_-vmg-lr9af07359281567eeb272ff0000a566aa.pdf?sfvrsn=13

// #define NumberOfAutomaticGears 6 // <<------- Select 3, 4 or 6 gears! // Relocated to vehicle adjustments!

// Reverse, 1st, 2nd, 3rd, 4th gear etc. (adjust reverse to match your ESC reverse speed)
#if NumberOfAutomaticGears == 6
uint8_t torqueconverterSlipPercentage = 100;
#ifndef OVERDRIVE
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 37, 29, 22, 17, 13, 10}; // x1.30 (Allison 3200 EVS (10, 54, 26, 22, 15, 12, 10) has too much spread)
#else
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 33, 25, 19, 14, 11, 8}; // Overdrive (lower RPM) in top gear. x1.33. OK, but fast shifting
#endif

#elif NumberOfAutomaticGears == 4
uint8_t torqueconverterSlipPercentage = 100;
#ifndef OVERDRIVE
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 43, 23, 14, 10}; // GM Turbo HydraMatic 700-R4
#else
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 30, 16, 10, 7}; // Overdrive (lower RPM) in 4th gear
#endif

#elif NumberOfAutomaticGears == 3
uint8_t torqueconverterSlipPercentage = 100;
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 25, 15, 10}; // GM Turbo HydraMatic 400

#else
uint8_t torqueconverterSlipPercentage = 100;
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 10}; // Construction vehicles with hydrostatic drive and just one forward gear
#endif

// Virtual 3 speed transmission gear ratios (times 10!, for example 15 means 1.5) ----------------
// Use it, if your vecicle does't have a real shifting transmission and you still want to shift
// your gears manually, using the 3 position switch
#if defined VIRTUAL_3_SPEED
int32_t virtualManualGearRatio[5] = {10, 23, 14, 10, 8}; // unused, 1st, 2nd, 3rd, (4rd overdrive) gear 23, 14, 10, 8
#endif

// Virtual 16 speed sequential transmission gear ratios (times 10!, for example 15 means 1.5) ----------------
// Use it, if your vecicle does't have a real shifting transmission and you still want to shift
// your gears manually, using the 3 position switch (up & down impulses)
#if defined VIRTUAL_16_SPEED_SEQUENTIAL
// int32_t virtualManualGearRatio[17] = {10, 154, 128, 107, 89, 74, 62, 52, 43, 36, 30, 25, 21, 17, 14, 12, 10}; // unused, 1st, 2nd etc.
// int32_t virtualManualGearRatio[17] = {10, 77, 64, 53, 44, 37, 31, 26, 21, 18, 15, 12, 10, 8, 7, 6, 5}; // unused, 1st, 2nd etc.
int32_t virtualManualGearRatio[17] = {10, 77, 64, 53, 44, 37, 31, 26, 21, 18, 15, 12, 10, 8, 7, 6, 5}; // unused, 1st, 2nd etc.
#endif

// Real manual 3 speed transmission --------------------------------------------------------------
// Use it in combination with a real 3 speed shifting transmission like the one from TAMIYA

#if defined HIGH_SLIPPINGPOINT // Curve for cars (high clutch engaging rpm)
float curveLinear[][2] = {
    {0, 0} // {input value, output value}
    ,
    {83, 200},
    {166, 260},
    {250, 320},
    {333, 380},
    {416, 440},
    {500, 500},
    {600, 500}, // overload range > 500 will limit output to 500
    {700, 500},
    {800, 500},
    {900, 500},
    {1000, 500},
    {1100, 500}
};

#else // Curve for heavy trucks (low clutch engaging rpm)
float curveLinear[][2] = {
    {0, 0} // {input value, output value}
    ,
    {83, 120},
    {166, 196},
    {250, 272},
    {333, 348},
    {416, 424},
    {500, 500},
    {600, 500}, // overload range > 500 will limit output to 500
    {700, 500},
    {800, 500},
    {900, 500},
    {1000, 500},
    {1100, 500}
};
#endif

//
// =======================================================================================================
// ARRAYS FOR QUICRUN FUSION MOTOR/ESC LINEARITY COMPENSATION
// =======================================================================================================
//

float curveQuicrunFusion[][2] = {
    {0, 0} // {input value, output value}
    ,
    {1000, 1000},
    {1100, 1050},
    {1200, 1110},
    {1300, 1170},
    {1400, 1270}

    ,
    {1500, 1500} // Neutral

    ,
    {1600, 1730},
    {1700, 1830},
    {1800, 1890},
    {1900, 1950},
    {2000, 2000},
    {3000, 3000} // overload range
};

// =======================================================================================================
// ARRAYS FOR QUICRUN 16BL30 ESC LINEARITY COMPENSATION
// =======================================================================================================
//

float curveQuicrun16BL30[][2] = {
    {0, 0} // {input value, output value}
    ,
    {1000, 1000},
    {1100, 1150},
    {1200, 1290},
    {1300, 1410},
    {1400, 1470} // 1470

    ,
    {1500, 1500} // Neutral

    ,
    {1600, 1530} // 1530
    ,
    {1700, 1590},
    {1800, 1710},
    {1900, 1850},
    {2000, 2000},
    {3000, 3000} // overload range
};

//
// =======================================================================================================
// ARRAY FOR EXPONENTIAL THROTTLE COMPENSATION
// =======================================================================================================
//

float curveExponentialThrottle[][2] = {
    {0, 0} // {input value, output value}
    ,
    {1000, 1000},
    {1100, 1150},
    {1200, 1290},
    {1300, 1410},
    {1400, 1470} // 1470

    ,
    {1500, 1500} // Neutral

    ,
    {1600, 1530} // 1530
    ,
    {1700, 1590},
    {1800, 1710},
    {1900, 1850},
    {2000, 2000},
    {3000, 3000} // overload range
};

//
// =======================================================================================================
// ARRAY INTERPOLATION FUNCTION
// =======================================================================================================
//

// Credit: http://interface.khm.de/index.php/lab/interfaces-advanced/nonlinear-mapping/

  uint32_t reMap(float pts[][2], uint32_t input) {
  uint32_t rr = 0;
  float mm = 0;

  for (uint8_t nn = 0; nn < 13; nn++) {
    if (input >= pts[nn][0] && input <= pts[nn + 1][0]) {
      mm = ( pts[nn][1] - pts[nn + 1][1] ) / ( pts[nn][0] - pts[nn + 1][0] );
      mm = mm * (input - pts[nn][0]);
      mm = mm +  pts[nn][1];
      rr = mm;
    }
  }
  return (rr);
}
