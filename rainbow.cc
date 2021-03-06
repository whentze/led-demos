#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

#define COLORS     2
#define NUM        1000
#define SPEED      0.4
#define OFFSET     0.2
#define ACCEL      0.9995
#define TURNSPEED  0.3
#define GSPEED     0.0003
#define SPOKES     5
#define PULSELEN   1
#define DECAY      0.95

#define WIDTH  6
#define HEIGHT 1

static int leds[WIDTH*32][HEIGHT*32][3];
static double gturn;
static double gshift;
static unsigned int pulse;

typedef struct {
  public:
    double x;
    double y;
    double xs;
    double ys;
    int r;
    int g;
    int b;
    bool held;
} star;

void initcolors();

static void DrawFull(Canvas *canvas){
  for(int x = 0; x < WIDTH*32; x++) {
    for(int y = 0; y < HEIGHT*32; y++) {
      canvas->SetPixel(x, y, leds[x][y][0], leds[x][y][1], leds[x][y][2]);
      leds[x][y][0] *= DECAY;
      leds[x][y][1] *= DECAY;
      leds[x][y][2] *= DECAY;
    }
  }
}

static double hues[COLORS][3];

void DrawOnCanvas(Canvas *canvas) {
  star stars[NUM];

  srand(time(NULL));

  while(1){

    gturn = fmod(gturn + cos(47*gshift)*cos(17*gshift)*TURNSPEED, 2*M_PI);
    gshift = fmod(gshift + GSPEED, 2*M_PI);
    pulse = (pulse + 1) % PULSELEN;
    initcolors();
    for(int i = 0; i < NUM; i++) {
      star* s = &(stars[i]);
      if(!s->held) {
        s->x += s->xs;
        s->y += s->ys;
        s->xs *= ACCEL;
        s->ys *= ACCEL;
        int ix = (int)(s->x + WIDTH*16 - 1);
        int iy = (int)(s->y + HEIGHT*16 - 1);
        if(0<=ix &&ix< WIDTH*32 && 0 <= iy && iy < HEIGHT*32 && fabs(s->xs)+fabs(s->ys) >= 0.002) {
          leds[ix][iy][0] = min(255, leds[ix][iy][0] + s->r);
          leds[ix][iy][1] = min(255, leds[ix][iy][1] + s->g);
          leds[ix][iy][2] = min(255, leds[ix][iy][2] + s->b);
        } else {
          s->held = true;
        }
      } else {
        if(!pulse){
          double dir = (double)rand();
          int col = (int)(fmod(SPOKES * dir + gturn, 2*M_PI)/(2*M_PI) * (COLORS));
          stars[i] = {
            48*sin(3*gshift)*sin(11*gshift),
            12*cos(7*gshift),
            (sin(dir) + 0.05*(7*sin(14*gshift) - 4*sin(8*gshift))) * SPEED,
            (cos(dir) - 0.05*(7/4*sin(7*gshift))) * SPEED,
            (int)hues[col][0],
            (int)hues[col][1],
            (int)hues[col][2],
            false,
          };
        }
      }
    }


    DrawFull(canvas);

    usleep(5000);
  }
}

void initcolors(){
  for(int col = 0; col < COLORS; col++) {
    double ahue = 13*gshift + col*2*M_PI/COLORS;
    hues[col][0] =  max(0.0, 100.0 * cos(ahue));
    hues[col][1] =  max(0.0, 100.0 * cos(ahue + 2*M_PI/3));
    hues[col][2] =  max(0.0, 100.0 * cos(ahue + 4*M_PI/3));
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

  initcolors();
  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
