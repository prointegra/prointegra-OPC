/*
 *  Prointegra OPC
 *
 *  Copyright 2016-2018 by it's authors. 
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
@File piSlave.h
*/

#ifndef _PISLAVE_
#define _PISLAVE_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "config.h"
#include "lib/gStools/libgStools.h"



/*! slave interface class*/
class SlaveInterface
{
 public:
  SlaveInterface(){return;};
  ~SlaveInterface(){delete rlMODBUS; return;};

  int setup(mbSlaves slaveParameters);
  //rlib communications settings
  int setupCommDaemon();
  int setupMBUSTCP();
  //read data
  int readData();
  int readTag(int index);
  int readINT(int index);
  int readWORD(int index);
  //write data
  int writeTag(int index, int slaveAmI, int value);
  //return functions
  int retNumTags(){return parameters.nRegs;};
  char * retTagName(int tag);
  int retTagValue(int tag);
  int retTagValid(int tag);
 private:
  mbSlaves parameters;
  //daemons
  rlModbusClient*    rlMODBUS = NULL;
};




#endif
