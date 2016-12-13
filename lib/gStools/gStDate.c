/*
#    gStools : gentooza SCADA tools
#
#    It's a library where put together all functions needed when developing free software SCADA or industrial control
#    software. (normally using pvbrowser)
#
#    'genSCADAtools' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#    'genSCADAtools' is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gStDate.h"

/*!time to Julian Int*/
/*!  Reference: Fliegel, H. F. & van Flandern, T. C. 1968, Communications of the ACM, 11, 657. 
inputs:
**time = pointer to time struct
outputs:
 integer as julian date
*/
int gstTime2JulianI(struct tm **time)
{
  int day,month,year;
  struct tm *lTime;

  lTime = *time;
  day = lTime->tm_mday;
  month = lTime->tm_mon;
  year = lTime->tm_year;


 return  day + (153 * (month+1 + 12 * ((13 - month) / 12) - 3) + 2) / 5 + 365 *(year + 6700 - ((13 - month) / 12)) + (year + 6700 - ((13 - month) / 12)) / 4 - (year + 6700 - ((13 - month) / 12)) / 100 + (year + 6700 - ((13 - month) / 12)) / 400 - 32045;
  //  day + (153 * (month+1 + 12 * ((13 - month) / 12) - 3) + 2) / 5 + 365 *(year + 1900 + 4800 - ((14 - month+1) / 12)) + (year + 1900 + 4800 - ((14 - month+1) / 12)) / 4 - (year + 1900 + 4800 - ((14 - month+1) / 12)) / 100 + (year + 1900 + 4800 - ((14 - month+1) / 12)) / 400 - 32045; 
}
