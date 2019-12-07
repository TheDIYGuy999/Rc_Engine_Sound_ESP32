//
// =======================================================================================================
// NONLINEAR ARRAY FOR THROTTLE
// =======================================================================================================
//

// In order to optimize the motor RPM behaviour for your vehicle, just change the array below

float curveShifting[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 200}
  , {166, 500}
  , {250, 200} // Simulated shifting point
  , {333, 500}
  , {416, 200} // Simulated shifting point
  , {500, 500}
};

float curveLinear[][2] = { 
  {0, 0} // {input value, output value}
  , {83, 200}
  , {166, 260}
  , {250, 320} // Simulated shifting point
  , {333, 380}
  , {416, 440} // Simulated shifting point
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
