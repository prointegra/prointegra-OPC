/*
 *  Prointegra OPC
 *
 *  Copyright 2016-2018 by it's authors. 
 *
 *  Some rights reserved. See COPYING, AUTHORS.
 *  This file may be used under the terms of the GNU General Public
 *  License version 3.0 (or any later version of GPL) as published by the Free Software Foundation
 *  and appearing in the file COPYING included in the packaging of
 *  this file.
 *
 *  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 *  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/**
@file commDaemon.cpp
'''launching,checking,logging... communication daemon manager class'''
*/

#include "commDaemon.h"

/*! constructor, creates pipes for every slave*/
CommDaemon::CommDaemon(mbSlaves slaveParams)
{
  //std::cout << "DEBUG: (inside CommDaemon::CommDaemon) " << std::endl;
  slaveP = slaveParams;
  dType = 0;
  if (!strcmp(slaveP.commType,"MODBUSTCP"))
    {
      std::cout << "DEBUG: (inside CommDaemon::CommDaemon) is MODBUS TCP! "<< std::endl;
      dType=PROT_MODBUS_TCP;
      iniMBTCP();
    }
  else
    std::cout << "ERROR: protocol: " << slaveP.commType << " NOT DEFINED!!" << std::endl;
//std::cout << "DEBUG: (inside CommDaemon::CommDaemon) finished"<< std::endl;
 return;
}

CommDaemon::~CommDaemon()
{
  //to IMPROVE
  std::string myProcess;

  myProcess = "killall ";
  myProcess += slaveP.slaveName;
  system(myProcess.c_str());
  delete nPipe;
  
  return;
}

/*!function for setting ini files for daemons, thanks to internal copy of slaves parameters*/
int CommDaemon::iniDaemon()
{
  std::cout << "DEBUG: (inside CommDaemon::iniDaemons) " << std::endl;
  int failed = -1;
  
  if(dType == PROT_MODBUS_TCP)
    failed = iniMBTCP();
  
  return failed;
}

/*!function for setting ini files for a Modbus TCP/IP daemon, thanks to internal copy of slaves parameters*/
int CommDaemon::iniMBTCP()
{
  std::cout << "DEBUG: (inside CommDaemon::iniMBTCP) " << std::endl;
  int failed = -1; 
  char * iniFile = NULL;
  IniConfigurator iniConfig;
   
  iniFile = new char[strlen("./comm/modbus_client/.mkmodbus")+10 + strlen(slaveP.slaveName)];
  sprintf(iniFile,"./comm/modbus_client/%s.mkmodbus",slaveP.slaveName); 
  std::cout << "INFO: setting MODBUS TCP/IP client ini file..."<< std::endl; 
  failed = iniConfig.iniMBTCPCreate(iniFile,slaveP);

  delete iniFile;

  return failed;
}

/*!function for launching comm daemon
*/
int CommDaemon::launchDaemon()
{
  //std::cout << "DEBUG: (inside CommDaemon::launchDaemon)" << std::endl;
  
  int failed = -1, result = 0;
  int protocol = 0;

  pthread_t* slaveThread = declareThread(dType);
   
  if(dType == PROT_MODBUS_TCP)
    {
      std::cout << "DEBUG:(inside CommDaemon::launchDaemon) creating thread for:" << dType << std::endl;
      result = pthread_create(slaveThread, NULL, launchMBUS , (void *) &slaveP);
      failed = 0;
      if (result)
	{
	  std::cout << "ERROR:(inside CommDaemon::launchDaemon) unable to create thread!:" << result << std::endl;
	  failed = -1;
	}

    }

  return failed;
}

int CommDaemon::checkDaemon(int commId)
{
  int failed = 0;
  char message[4];
  
  //read (nPipes[slave][0], message, sizeof(message));
  //message[3] = '\0';
  //std::cout << "DEBUG: message string from daemon: " << message << std::endl;
  //if(strstr(message,"1")!=NULL)
  //  failed = 0;

  return failed;
}

pthread_t* CommDaemon::declareThread(int nSlave)
{
  if (threads.size() <= nSlave)
    {
      threads.resize(nSlave+1,NULL);
      threads.at(nSlave) = new pthread_t;
    }
  else
    {
      if(threads.at(nSlave) == NULL)
	threads.at(nSlave) = new pthread_t;
    }
  
  return  threads.at(nSlave);

}

////tools

int renameOldLog(char** logPath, const char * name)
{
  static char *file1,*file2 = NULL;
  int failed = -1;

  file1 = new char[110];
  file2 = new char[110];

  sprintf(file1,"./log/%s.log",name);
  sprintf(file2,"./log/%s_old.log",name);
  rename(file1, file2);
  failed = 0;
	
  *logPath = file1;
  delete file2;
  return 0;
}

int setExecutable(char **executable, const  char * name)
{
  std::cout << "DEBUG:(inside CommDaemon::setExecutable)"<< std::endl;
  int failed = -1;
  static char *file1 = NULL;

  file1 = new char[sizeof("./comm/modbus_client/") + strlen(name) + 40];
  sprintf(file1,"./comm/modbus_client/%s",name);
  failed = 0;

  *executable = file1;
  return failed;
}

void* launchMBUS(void * parameters)
{
  std::cout << "DEBUG:(inside CommDaemon::launchMBUS)"<< std::endl;
  char buffer[500];
  std::string result;
  char *tmpString = NULL;
  FILE *fout; // out file

  char timeBuffer[10];
  time_t rawTime;
  struct tm * timeInfo;
  int lines = 0;
  const int maxLines = 20000;
  mbSlaves *params = (mbSlaves*) parameters;
  
  renameOldLog(&tmpString,params->slaveName);
  fout = fopen(tmpString,"w");
  delete tmpString;

  if (!setExecutable(&tmpString, params->slaveName))
    {
      std::shared_ptr<FILE> pipe(popen(tmpString, "r"), pclose);
      if (!pipe) throw std::runtime_error("popen() failed!");
      std::cout << "DEBUG:(inside CommDaemon::launchMBUS) executable was launched: " << tmpString << std::endl;
      delete tmpString;
      while (!feof(pipe.get()))
	{
	  if (fgets(buffer, 500, pipe.get()) != NULL)
	    {
	      //time(&rawTime);
	      //timeInfo = localtime(&rawTime);
	      //strftime (timeBuffer,10,"%T: ",timeInfo);	      
	      //fprintf(fout,"%s%s", timeBuffer,buffer);
	      //lines++;
	      //if(lines >= maxLines)
	      //{
	      //  rewind(fout);
	      //  lines = 0;
	      //}
	    }
	}
    }
  else
    std::cout << "DEBUG:(inside CommDaemon::launchMBUS) executable was launched and has finished: " << tmpString << std::endl;
  pthread_exit(NULL);
  fclose(fout);
}
