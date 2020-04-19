# CosMotor
Library for moving a stepper motor following a 1-cos() curve

## Why follow a 1+cos() curve?

The upper plots in the following figure shows the position, velocity and accelerations corresponding
to a movement performed in a 1-cos() way.

The bottom traces correspond to the position and acceleration when the movement is done
with constant acceleration, followed by constant velocity, followed by constant decelaration

<p align="center">
  <img width="400" src="https://github.com/PerePala/CosMotor/blob/master/fig.png">
</p>

If you were riding in an elevator following the lower traces your apparent weight would be
changing abruptly: there is jerk. In contrast, the sinusoidal term, being infinitelly derivable,
gives very smooth movements.

## How to implement this movement?

Trigonometric functions are expensive de compute. Here, we use an [Octave script](https://github.com/PerePala/CosMotor/blob/master/data.m) to write out a [table](https://github.com/PerePala/CosMotor/blob/master/s.txt). 
This table produces a piecewise-linear approximation to the sinusoindal movement.

## How to use this?

1. Instantiate a *CosMotor* element 

1. Schedule it 

  * At some time
  * In the desired direction
  * With the desired amplitude (number of steps)
  * In the desired amount of time (milliseconds)
  * (And decide wether to keep the motor enabled after finishing)

Your main loop has to call *update* frequently and should not contain
blocking elements (no *delay* or similar).

A simple example is:

```C++
#include "CosMotor.h"
//                      ENA,DIR,STP
CosMotor cm = CosMotor(  8 , 6 , 3  );

uint8_t  mainDir = CW;
uint32_t tsched  = 7000;

void setup() {
  Serial.begin(115200);
  
  // Schedule a movement 2 seconds after start
  // Amplitude is 800 steps. Duration 1000 milliseconds
  // The motor is kept enabled after finishing
  cm.scheduleAt(2000 , mainDir = CW, 800, 1000,     true);
}

void loop() {
  // Call update frequently in the loop. This is non-blocking.
  cm.update();

  // DO NOT block main loop
  if (cm.motorState == FINISHED) {
    
    // Schedule alternated movements every 5 seconds from now
    // Modify as needed
    if (mainDir == CW) {
      cm.scheduleAt(tsched , mainDir = CCW, 4000,  800,  true);
    }
    else {
      cm.scheduleAt(tsched , mainDir =  CW, 2400, 1000, false);
    }
    tsched += 5000;
  }
}
```
