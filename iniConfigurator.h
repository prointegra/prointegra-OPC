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
@File iniConfigurator.h
*/

#ifndef _INI_CONFIGURATOR_
#define _INI_CONFIGURATOR_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "config.h"


class IniConfigurator
{
 public:
  IniConfigurator(){return;};
  ~IniConfigurator(){return;};

  int iniCreate(char * path, mbSlaves* slaveParams);
  int usingSocket(mbSlaves* parameters);
  int writCycles(FILE* pFile, mbSlaves* parameters);

};
#endif
