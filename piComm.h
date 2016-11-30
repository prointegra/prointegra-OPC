/*
 *  Prointegra OPC
 *
 *  Copyright 2016 by it's authors. 
 *
 *  Some rights reserved. See COPYING, AUTHORS.
 *  This file may be used under the terms of the GNU General Public
 *  License version 3.0 as published by the Free Software Foundation
 *  and appearing in the file COPYING included in the packaging of
 *  this file.
 *
 *  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 *  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*!
@File piComm.h
*/

#ifndef _PICOMMUNICATIONS_
#define _PICOMMUNICATIONS_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "config.h"



/*! slave interface class*/
class CommInterface
{
 public:
  CommInterface(){return;};
  ~CommInterface();

  int setup(mbSlaves slaveParameters);

 private:
  mbSlaves parameters;
};




#endif
