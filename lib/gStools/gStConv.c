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

#include "gStConv.h"

/*from modbus word conversions*/
/*!word to int conversion
inputs:
 word = word to convert
outputs:
 integer converted
*/

int gstWord2Int(int word)
{
  int value;
  
  if(word < 0 || word > 65535)
    return 0; //out of range
  if(word < 32768) //positive
    return word;
  //converting negative
  value = word^65535;
  value += 1;
  value = -value;
  return value;
}

/*!words to dint conversion
inputs:
 lWord = lower word to convert
 hWord = higher word to convert
outputs:
 double integer converted
*/

long gstDWord2Long(int lWord, int hWord)
{
  long value;
  long temp;
  
  if((lWord < 0 || lWord > 65535) || (hWord < 0 || hWord > 65535))
    return 0; //out of range
  if(hWord < 32768)
    {//positive
      value = hWord * pow(2,16);
      return value + lWord;
    }
  //converting negative
  value = hWord^65535;
  value = value * pow(2,16);
  temp = lWord^65535;
  temp += 1;
  if(temp == 65536)
    {
      temp = 0;
      value = value+1;
    }
  else
      value = value + temp;
  
  value = -value;
  return value;
}

/*!words to float conversion
inputs:
 lWord = lower word to convert (LSW)
 hWord = higher word to convert (MSW)
outputs:
 double float converted
*/

long double gstDWord2Float(int lWord, int hWord)
{
  long double value = 0.0;
  int sign = 1;
  int exponent;
  int mantissa;
  int temp;
  int i;

  if((lWord < 0 || lWord > 65535) || (hWord < 0 || hWord > 65535))
    return 0; //out of range
  if(hWord > 32768)
    {//negative
      sign = - 1;
    }
  exponent = hWord >> 7;
  exponent = exponent & 0xFF;
  exponent -= 127;

  mantissa = (hWord & 0x7F) << 16;
  mantissa += lWord;

  for(i = 23 ; i >= 0 ; i--)
    {
      temp = mantissa >> i;
      temp = temp &1;
      value += temp * pow(2,i-23);
    }

  value = (1+value) * pow(2,exponent);
  value = value*sign;
  return value;
  
}

///////
/*to modbus word conversions
inputs:
 integer = integer to convert
outputs:
 integer as word value
*/

int gstInt2Word(int integer)
{
  int value;
  
  //check integer limits
  if (integer > 32767)
    integer = 32767;
  if (integer < -32768)
    integer = -32768;
  //negative
  if (integer < 0)
      value = ((-integer) ^ 65535) +1;
  else
    value = integer;
  
  return value;
}
