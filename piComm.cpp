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
@File piComm.cpp
'''communication manager class, to manage slaves'''
*/

#include "piComm.h"


/*! function to take slave connection parameters and build interface dinamically */
int CommInterface::setup(mbSlaves slaveParams)
{

  int ret;
  
  //taking db parameters
  parameters = slaveParams;
  if(parameters.isValid)
    {
      std::cout << "DEBUG: parameters valid!" << std::endl;
    }

  return 0;
}
