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


#ifndef _TIMER_CPP_
#define _TIMER_CPP_

#include "timer.hpp"

CTimer::CTimer():n(0),
                 max_time(0),
                 min_time(99999999),
                 avg_time(0),
                 total_time(0),
                 cur_time(0)
{}

void CTimer::Start()
{
  cur_time = (double)clock()/CLOCKS_PER_SEC;
}

void CTimer::Stop()
{
  cur_time = (double)clock()/CLOCKS_PER_SEC - cur_time;

  n++;
  total_time += cur_time;
  if(cur_time > max_time) max_time = cur_time;
  if(cur_time < min_time) min_time = cur_time;
}

void CTimer::Reset()
{
  n          = 0;
  max_time   = 0;
  min_time   = 99999999;
  avg_time   = 0;
  total_time = 0;
  cur_time   = 0;
}

double CTimer::Max()
{
  return max_time;
}
 
double CTimer::Min()
{
  return min_time;
}

double CTimer::Avg()
{
  return total_time/n;
}                

double CTimer::Total()
{
  return total_time;
}

#endif
