/*    
  Library for moving a stepper motor following a 1-cos() curve.
   
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

   Instantiate with
   CosMotor(ENA_PIN, DIR_PIN, STP_PIN);

   Schedule a movement at a given time
   Specify: number of steps, duration (ms), direction

   Read the state with motorState
   Read the number of CW and CCW steps with totalCWSteps/totalCCWSteps  
*/

#ifndef CosMotor_h
#define CosMotor_h

//#include "Arduino.h"

# define CW    LOW
# define CCW   HIGH

# define nTargets 11


enum states {INACTIVE, SCHEDULED, ACTIVE, FINISHED};
struct table_t {
  uint16_t s;  // number of steps
  uint16_t p;  // period in us
};


class CosMotor
{
  public:
    CosMotor(uint8_t ena, uint8_t dir, uint8_t stp);
    void scheduleAt(uint32_t t_ms,   // schedule time in ms
                    uint8_t dir,          // CW / CCW
                    uint16_t nSteps, // how many steps: 800 = 1 rev
                    uint16_t dur_ms, // how long lasts in ms
                    boolean keep_enabled); // keep motor enabled after end?
    void update();
    enum states motorState = INACTIVE;
    uint32_t totalCWSteps, totalCCWSteps;

  private:
    uint8_t  _table_index;
    uint32_t _table_startTime;
    boolean  _table_keepEnabled;
    uint16_t _table_nSteps;
    uint16_t _table_dur_ms;
    uint8_t  _ena;
    uint8_t  _dir;
    uint8_t  _stp;
    uint32_t _time0;
    uint32_t _time1;
    uint32_t _step_us; // microseconds
    uint32_t _totalSteps;
    uint8_t  _currDir; // current direction

    table_t  _table[(nTargets + 1) / 2];


    void setDir(uint8_t val);

    void doStep();
    void resetCounters();
//    void table_activate();
    void activate();
    uint8_t table_map(uint8_t ix);
    void table_update();
    void enable(boolean enable);
    uint8_t imap(uint8_t ix);


};

#endif
