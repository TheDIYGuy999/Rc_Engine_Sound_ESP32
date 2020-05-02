//
// =======================================================================================================
// ARRAYS FOR THROTTLE
// =======================================================================================================
//

// In order to optimize the motor RPM behaviour for your vehicle, just change the array below

// Simulating a 4 speed automatic transmission, using a sawtooth array
float curveAutomatic[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 350} // Initial torque converter slip
  , {187, 500}
  , {188, 250} // Simulated shifting point to 2nd gear
  , {291, 500}
  , {292, 250} // Simulated shifting point to 3rd gear
  , {396, 500}
  , {397, 250} // Simulated shifting point to 4rd gear
  , {500, 500}
  , {600, 500} // overload range > 500 will limit output to 500
};

/*
// Simulating a 3 speed automatic transmission, using a sawtooth array
float curveAutomatic[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 350} // Initial torque converter slip
  , {222, 500}
  , {223, 250} // Simulated shifting point to 2nd gear
  , {361, 500}
  , {362, 250} // Simulated shifting point to 3rd gear
  , {500, 500}
  , {600, 500} // overload range > 500 will limit output to 500
};*/

// Use it in combination with a real 3 speed shifting transmission like the one from TAMIYA
float curveLinear[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 200}
  , {166, 260}
  , {250, 320}
  , {333, 380}
  , {416, 440}
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
