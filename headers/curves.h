//
// =======================================================================================================
// ARRAYS FOR ENGINE RPM
// =======================================================================================================
//

// Automatic transmission gear ratios (times 10!, for example 15 means 1.5) -----------------------
// see: https://www.allisontransmission.com/docs/default-source/marketing-materials/sa7943en_-2017-vocational-model-guide_-vmg-lr9af07359281567eeb272ff0000a566aa.pdf?sfvrsn=13

//#define NumberOfAutomaticGears 6 // <<------- Select 3, 4 or 6 gears! // Relocated to vehicle adjustments!

//Reverse, 1st, 2nd, 3rd, 4th gear etc. (adjust reverse to match your ESC reverse speed)
#if NumberOfAutomaticGears == 6
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 48, 32, 22, 15, 12, 10}; // Allison 3200 EVS (10, 54, 26, 22, 15, 12, 10)
#elif NumberOfAutomaticGears == 4
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 44, 23, 14, 10}; // GM Turbo HydraMatic 700-R4
#elif NumberOfAutomaticGears == 3
int32_t gearRatio[NumberOfAutomaticGears + 1] = {10, 25, 15, 10}; // GM Turbo HydraMatic 400
#endif


// Virtual 3 speed transmission gear ratios (times 10!, for example 15 means 1.5) ----------------
// Use it, if your vecicle does't have a real shifting transmission and you still want to shift
// your gears manually, using the 3 position switch
#if defined VIRTUAL_3_SPEED
int32_t virtualManualGearRatio[4] = {10, 23, 14, 10}; // unused, 1st, 2nd, 3rd gear 32, 17, 10
#endif

// Virtual 16 speed sequential transmission gear ratios (times 10!, for example 15 means 1.5) ----------------
// Use it, if your vecicle does't have a real shifting transmission and you still want to shift
// your gears manually, using the 3 position switch (up & down impulses)
#if defined VIRTUAL_16_SPEED_SEQUENTIAL
//int32_t virtualManualGearRatio[17] = {10, 154, 128, 107, 89, 74, 62, 52, 43, 36, 30, 25, 21, 17, 14, 12, 10}; // unused, 1st, 2nd etc.
//int32_t virtualManualGearRatio[17] = {10, 77, 64, 53, 44, 37, 31, 26, 21, 18, 15, 12, 10, 8, 7, 6, 5}; // unused, 1st, 2nd etc.
int32_t virtualManualGearRatio[17] = {10, 77, 64, 53, 44, 37, 31, 26, 21, 18, 15, 12, 10, 8, 7, 6, 5}; // unused, 1st, 2nd etc.
#endif


// Real manual 3 speed transmission --------------------------------------------------------------
// Use it in combination with a real 3 speed shifting transmission like the one from TAMIYA
// Curve for cars (high clutch engaging rpm)
/*
float curveLinear[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 200}
  , {166, 260}
  , {250, 320}
  , {333, 380}
  , {416, 440}
  , {500, 500}
  , {600, 500} // overload range > 500 will limit output to 500
};*/

// Curve for heavy trucks (low clutch engaging rpm)
float curveLinear[][2] = {
    {0, 0} // {input value, output value}
    , {83, 120}
    , {166, 196}
    , {250, 272}
    , {333, 348}
    , {416, 424}
    , {500, 500}
    , {600, 500} // overload range > 500 will limit output to 500
};

//
// =======================================================================================================
// ARRAY INTERPOLATION
// =======================================================================================================
//

// Credit: http://interface.khm.de/index.php/lab/interfaces-advanced/nonlinear-mapping/

uint32_t reMap(float pts[][2], uint32_t input) {
  uint32_t rr;
  float bb, mm;

  for (uint8_t nn = 0; nn < 12; nn++) {
    if (input >= pts[nn][0] && input <= pts[nn + 1][0]) {
      mm = ( pts[nn][1] - pts[nn + 1][1] ) / ( pts[nn][0] - pts[nn + 1][0] );
      mm = mm * (input - pts[nn][0]);
      mm = mm +  pts[nn][1];
      rr = mm;
    }
  }
  return (rr);
}
