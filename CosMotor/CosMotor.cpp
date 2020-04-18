/*    
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
*/

#include "Arduino.h"
#include "CosMotor.h"

//#define DEBUG

CosMotor::CosMotor(uint8_t ena, uint8_t dir, uint8_t stp) {
  _ena = ena;
  _dir = dir;
  _stp = stp;
  pinMode(_ena, OUTPUT);
  pinMode(_dir, OUTPUT);
  pinMode(_stp, OUTPUT);
  setDir(CW);
  resetCounters();

#ifdef DEBUG
  Serial.println("Motor pins ena,dir,stp:");
  Serial.println(_ena);
  Serial.println(_dir);
  Serial.println(_stp);
#endif
};

void CosMotor::scheduleAt(
      uint32_t t_ms,         // schedule time in ms
      uint8_t  dir,          // CW / CCW
      uint16_t nSteps,       // how many steps: 800 = 1 rev
      uint16_t dur_ms,       // how long lasts in ms
      boolean  keep_enabled) // keep motor enabled after end?
{
  // Build a table with a quarter period of the sine wave
  const table_t baseTable[(nTargets + 1) / 2] = {
    // step,period
    {   16 , 5682 }, 
    {   47 , 1934 }, 
    {   75 , 1212 }, 
    {   96 ,  947 }, 
    {  109 ,  834 }, 
    {  114 ,  797 },  
  };

  _table_startTime = t_ms;
  setDir(dir);
  motorState = SCHEDULED;
  _table_nSteps = nSteps;
  _table_dur_ms = dur_ms;
  _table_keepEnabled = keep_enabled;

  // Compute the required values for the target range and duration
  uint16_t sumSteps = 0;
  uint16_t Steps = 0;
  // Loop up to the last element. If nTargets=11 -> 1..5
  for (int i = 0; i < (nTargets - 1) / 2; i++) {
    Steps = (uint32_t) nSteps * (uint32_t)baseTable[i].s / (uint32_t) 800;
    sumSteps += Steps;
    _table[i].s = Steps;
    _table[i].p = ((uint32_t) dur_ms * (uint32_t) baseTable[i].p * (uint32_t) 4) / 
                  ((uint32_t) 5 * (uint32_t) nSteps);
  }
  // Now, the last (fastest) segment: 
  // Do not compute but take the difference so that the total number of steps is nSteps
  _table[(nTargets - 1) / 2].s = nSteps - 2 * sumSteps; // This will be always positive
  // TODO: This way, we get a small change in duration. A few steps of the fastest segment
  _table[(nTargets - 1) / 2].p = ((uint32_t) dur_ms * (uint32_t) baseTable[(nTargets - 1) / 2].p * (uint32_t) 4) /
                                 ((uint32_t) 5 * (uint32_t) nSteps);

#ifdef DEBUG
  for (int i = 0; i < (nTargets + 1) / 2; i++) {
    Serial.print(_table[i].s);
    Serial.print(" : ");
    Serial.println(_table[i].p);
  }
#endif

//  lcd.setCursor(0, 0);
//  lcd.print(table_startTime);

  _step_us = _table[0].p;
}
//------------------------------------------------------------------------
void CosMotor::activate() {
  enable(true);
  motorState = ACTIVE;
  resetCounters();
  _table_index = 0;
  _step_us = _table[0].p;

#ifdef DEBUG
  Serial.print("A");
  Serial.println(millis());
#endif

//  lcd.setCursor(0, 1);
//  lcd.print("A");
}

uint8_t CosMotor::imap(uint8_t ix) {
  if (ix < (nTargets + 1) / 2) {
    return ix;
  }
  else {
    return nTargets - ix - 1;
  }
}

//------------------------------------------------------------------------
void CosMotor::update() {
  
  // 1. Do motor steps
  _time1 = micros();
  if ((_time1 - _time0) >= _step_us) {
    doStep();
    _time0 = _time1; // TODO: think if this has to be only when table_active
  }

  // 2. Do update table if necessary
  if (motorState == ACTIVE) {
    if (_totalSteps >= _table[imap(_table_index)].s) { //Done all steps?
      _totalSteps = 0;
      _table_index++;
      if (_table_index < nTargets) {
        _step_us = _table[imap(_table_index)].p;
        //        Serial.println(step_us);
      }
      else {
        enable(_table_keepEnabled);
        motorState = FINISHED;
        //        lcd.setCursor(0, 1);
        //        lcd.print("F");
      }
    }
  }
  else { //see if it has to become active
    if ((motorState = SCHEDULED) && (millis() >= _table_startTime)) {
      activate();
    }
  }
}


void CosMotor::resetCounters() {
  totalCWSteps = 0;
  totalCCWSteps = 0;
}

void CosMotor::enable(boolean enable) {
  if (enable) {
    digitalWrite(_ena , LOW );
  }
  else {
    digitalWrite(_ena , HIGH );
  }
}

void CosMotor::setDir(uint8_t val) {
  _currDir = val;
  digitalWrite(_dir, _currDir);
}
//------------------------------------------------------------------------
void CosMotor::doStep() {
  if (motorState == ACTIVE) {
    digitalWrite(_stp, HIGH);
    delayMicroseconds(2);
    digitalWrite(_stp, LOW);
    delayMicroseconds(2);
    if (_currDir == CW) {
      totalCWSteps++;
    }
    else {
      totalCCWSteps++;
    }
    _totalSteps++;
  }
}
