//
// =======================================================================================================
// ARRAYS FOR THROTTLE
// =======================================================================================================
//

// In order to optimize the motor RPM behaviour for your vehicle, just change the array below

// Simulating a 3 speed automatic transmission, using a sawtooth array
float curveAutomatic[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 166}
  , {222, 500}
  , {223, 166} // Simulated shifting point
  , {361, 500}
  , {362, 166} // Simulated shifting point
  , {500, 500}
};

// Use it in combination with a real 3 speed shifting transmission like the one from TAMIYA
float curveLinear[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 200}
  , {166, 260}
  , {250, 320}
  , {333, 380}
  , {416, 440}
  , {500, 500}
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

  for (uint8_t nn = 0; nn < 7; nn++) {
    if (input >= pts[nn][0] && input <= pts[nn + 1][0]) {
      mm = ( pts[nn][1] - pts[nn + 1][1] ) / ( pts[nn][0] - pts[nn + 1][0] );
      mm = mm * (input - pts[nn][0]);
      mm = mm +  pts[nn][1];
      rr = mm;
    }
  }
  return (rr);
}
