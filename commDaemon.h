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
/**
@file commDaemon.h
*/

#ifndef _COMMDAEMON_
#define _COMMDAEMON_

#include <iostream>
#include <string.h>
#include <vector>
#include <memory>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <vector>

#include <unistd.h>

#include "config.h"
#include "iniConfigurator.h"

enum PROTOCOLS
  {
    PROT_MODBUS_TCP = 1
  };

//tools
int renameOldLog(int commId,char** logPath);
int setExecutable(int commId,char* protocol, char ** executable);
void* launchMBUS(void* commId);

/*launching,checking,logging... communication daemon manager class*/
class CommDaemon
{
 public:
  CommDaemon(std::vector <mbSlaves> slavesParams);
  ~CommDaemon();
  
  int iniDaemons();
  int iniMBTCP();
  
  int launchDaemon(int slave, int commId, char* protocol);
  int checkDaemon(int slave);
  //class tools
  int isDefined(int protocol)
  //threads
  pthread_t* declareThread(int nSlave);
  
 private:
  int **nPipes;
  int nSlaves;
  std::vector<pthread_t*> threads;
  std::vector <mbSlaves> slavesP;
  std::vector <int> daemons;
};
#endif
