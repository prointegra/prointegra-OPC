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

/*! main header file */

#ifndef __GST_CONV__
#define __GST_CONV__
#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C"{
#endif
  
/*!conversion word to int*/
int gstWord2Int(int word);
/*!conversion double word to long*/
long gstDWord2Long(int lWord, int hWord);
/*!conversion double word to float*/
long double gstDWord2Float(int lWord, int hWord);
/////  
/*!conversion int to word*/
int gstInt2Word(int integer);
  
#ifdef __cplusplus
}
#endif

#endif
