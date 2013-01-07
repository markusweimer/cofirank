/* Copyright (c) 2006, National ICT Australia
 * All rights reserved.
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Authors      : Choon Hui Teo (ChoonHui.Teo@anu.edu.au)
 * Created      : 08/01/2007
 * Last Updated :
 */


#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <time.h>

/** Class for recording cpu-seconds of program segments.
 */
class CTimer {

protected:

  long   n;             // number of records
  double max_time;      // longest recorded time interval
  double min_time;      // shortest recorded time interval
  double avg_time;      // average time interval recorded
  double total_time;    // total time interval recorded
  double cur_time;      // starting time

public:

  CTimer();             // constructor
  virtual ~CTimer(){}   // destructor

  void   Start();       // start recording time
  void   Stop();        // stop recording time
  void   Reset();       // reset internal time statistics
  double Max();         // return max_time
  double Min();         // return min_time
  double Avg();         // compute and return avg_time
  double Total();       // return total_time
  
};

#endif
