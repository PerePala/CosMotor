/*
  This sketch is an example of using CosMotor.

    Copyright (C) 2020  Pere Pala-Schonwalder

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    You can contact the author at https://github.com/PerePala/CosMotor
  
  For this example, we assume an
  DRV8825 DRIVER in the Y slot of the CNC Board

  This sketch produces a movement of the form 1-cos(). 
  Sinusoidal functions have an infinite number of continuous derivatives.
  This property gives a smooth motion.
  
  These movements 
   1. Are scheduled at a specific time given in milliseconds.
   2. Are produced in CW/CCW direction
   3. Have the desired rang of motion (in steps)
   4. Have the desired duration (in milliseconds)
   5. Can end with the motor enabled (or not)

   If the motor is kept enabled, it actively holds the position (and
   consumes power). 
*/

#include "CosMotor.h"

//                      ENA,DIR,STP
CosMotor cm = CosMotor(  8 , 6 , 3  );

uint8_t  mainDir = CW;
uint32_t tsched  = 7000;

//========================================================================
void setup() {
  
  Serial.begin(115200);

  // Schedule a movement 2 seconds after start
  // Amplitude is 800 steps. Duration 1000 milliseconds
  // The motor is kept enabled after finishing
  cm.scheduleAt(2000 , mainDir = CW, 800, 1000,     true);
}

//========================================================================

void loop() {

  // Call update frequently in the loop. This is non-blocking.
  cm.update();

  // DO NOT block main loop
  if (cm.motorState == FINISHED) {
    
    Serial.println("Time / CW / CCW ");
    Serial.print(micros());
    Serial.print(" / ");
    Serial.print(cm.totalCWSteps);
    Serial.print(" / ");
    Serial.println(cm.totalCCWSteps);

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
