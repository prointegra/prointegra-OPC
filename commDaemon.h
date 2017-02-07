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

#include <unistd.h>

//tools
int renameOldLog(int commId,char** logPath);
int setExecutable(int commId,char* protocol, char ** executable);

/*launching,checking,logging... communication daemon manager class*/
class CommDaemon
{
 public:
  CommDaemon(int slaves);
  ~CommDaemon();

  int launchDaemon(int slave, int commId, char* protocol);
  static void* launchMBUS(void* commId);
  int checkDaemon(int slave);
  //threads
  pthread_t* declareThread(int nSlave);
  
 private:
  int **nPipes;
  int nSlaves;
  std::vector<pthread_t*> threads;
  
};
#endif
