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

//
// =======================================================================================================
// ARRAY INTERPOLATION
// =======================================================================================================
//

// Credit: http://interface.khm.de/index.php/lab/interfaces-advanced/nonlinear-mapping/

int reMap(float pts[][2], int input) {
  int rr;
  float bb, mm;

  for (int nn = 0; nn < 7; nn++) {
    if (input >= pts[nn][0] && input <= pts[nn + 1][0]) {
      mm = ( pts[nn][1] - pts[nn + 1][1] ) / ( pts[nn][0] - pts[nn + 1][0] );
      mm = mm * (input - pts[nn][0]);
      mm = mm +  pts[nn][1];
      rr = mm;
    }
  }
  return (rr);
}
