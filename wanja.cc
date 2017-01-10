// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is wanja's domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define WIDTH 6
#define HEIGHT 1

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

static int leds[WIDTH*32][HEIGHT*32][3];

static float absin(float x) {
  float xx = fmod(x, 6.2831853);
  if (xx < 3.1415927) {
    return 1 - 2*fabs(xx/3.1415927 - 0.5);
  }
  return 2*fabs(xx/3.1415927 - 1.5) - 1;
}

static float abcos(float x) {
  return absin(x + 1.5707963);
}

static void DrawFull(Canvas *canvas){
  for(int x = 0; x < WIDTH*32; x++) {
    for(int y = 0; y < HEIGHT*32; y++) {
      canvas->SetPixel(x, y, leds[x][y][0], leds[x][y][1], leds[x][y][2]);
    }
  }
}

static void DrawOnCanvas(Canvas *canvas) {
  float s = 0;
  float p = 0;

  while(1){
    for(p = 0; p < 6.2831853; p += 0.001) {
      int r = min(0, (int)(64*absin(5*p+s*21)));
      int g = min(0, (int)(64*absin(5*p+s*31)));
      int b = min(0, (int)(64*absin(5*p+s*50)));

      float fx = WIDTH*16 + sin(p*1) * sin(151* s + 7* p + 98* s + 5 * cos(p+s))          * WIDTH *16;
      float fy = HEIGHT*16 + cos(p*3) * cos(1651*s + 27*p + 176*s + 7 * sin(p-s))         * HEIGHT*16;

      int x = max(0, min(WIDTH*32  - 1, int(fx)));
      int y = max(0, min(HEIGHT*32 - 1, int(fy)));

      //leds[x][y][0] = std::min(255, leds[x][y][0] + r);
      //leds[x][y][1] = std::min(255, leds[x][y][1] + g);
      //leds[x][y][2] = std::min(255, leds[x][y][2] + b);

      //gamma correction
      leds[x][y][0] = (int)std::min(255.0, sqrt(leds[x][y][0]*leds[x][y][0] + r*r));
      leds[x][y][1] = (int)std::min(255.0, sqrt(leds[x][y][1]*leds[x][y][1] + g*g));
      leds[x][y][2] = (int)std::min(255.0, sqrt(leds[x][y][2]*leds[x][y][2] + b*b));

    }

    s = fmod(s + 0.0001, 6.2831853);
    usleep(20000);
    DrawFull(canvas);

    memset(leds, 0, sizeof(leds));
  }
}

int main(int argc, char *argv[]) {
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;

  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  int rows = 32;    // A 32x32 display. Use 16 when this is a 16x32 display.
  int chain = WIDTH;    // Number of boards chained together.
  int parallel = HEIGHT; // Number of chains in parallel (1..3). > 1 for plus or Pi2
  Canvas *canvas = new RGBMatrix(&io, rows, chain, parallel);

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
