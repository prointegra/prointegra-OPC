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
@File capture.h
*/

#ifndef _CAPTURE_
#define _CAPTURE_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <signal.h>

#include <time.h>

#include "piDatabase.h"
#include "piComm.h"
#include "libgStools.h"
#include "config.h"

#include "commDaemon.h"


#ifdef __FUCKIN_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

static int lExit = 0;

/*!server capture class, it setup capture and run&check it */
class ProintegraOPC 
{
 public:
 ProintegraOPC();
  ~ProintegraOPC();
  //initialization functions
  int startCommunications();
  int initComm();
  //checking fucntions
  int checkDB(); 
  int checkComm();  
  //capturing functions
  int loop();
  int lockTables();
  int dataCapture();
  int dataToComm();
  int dataToDB();
  int storeDB();
  int getTriggers();
  int delTriggers();
  //tools
  int linkTags();
  //DEBUGGING FUNCTIONS
  int showDBData();
  int showDBDataLinkage();
  int showTriggers();
  //EXIT HANDLER
  static void exitHandler(int s){lExit = 1;};
  int stopComm();

  
 private:
  //database configuration parser
  ConfigParser* confParser = NULL;
  ////databases
  //number of database handlers
  int nDBs;
  //database handler
  DBInterface** hDatabase = NULL;
  ////communications
  //number of slaves handlers
  int nSlaves;
  CommInterface** hSlaves = NULL;
  CommDaemon* commDaemonManager = NULL;

};


#endif
