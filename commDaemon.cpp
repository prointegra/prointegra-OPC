/*
 *  Prointegra OPC
 *
 *  Copyright 2016,2017 by it's authors. 
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
CommDaemon::CommDaemon(std::vector <mbSlaves> slavesParams)
{
  std::cout << "DEBUG: (inside CommDaemon::CommDaemon) " << std::endl;
  slavesP.swap(slavesParams);
  daemons.clear();
  std::cout << "DEBUG: (inside CommDaemon::CommDaemon) vector size: " << slavesP.size() << std::endl;
  for(int i= 0; i < slavesP.size(); i++)
    {
      std::cout << "DEBUG: (inside CommDaemon::CommDaemon) checking slave nº: "<< i << std::endl;
      if (!strcmp(slavesP.at(i).commType,"MODBUSTCP"))
	{
	  std::cout << "DEBUG: (inside CommDaemon::CommDaemon) is MODBUS TCP! "<< std::endl;
	  if (!isDefined(PROT_MODBUS_TCP))
	    daemons.push_back(PROT_MODBUS_TCP);	    
	}
      else
	std::cout << "ERROR: protocol: " << slavesP.at(i).commType << " NOT DEFINED!!" << std::endl;
    }
	  std::cout << "DEBUG: (inside CommDaemon::CommDaemon) finished"<< std::endl;
  return;
}

CommDaemon::~CommDaemon()
{
  for(int i=nSlaves-1;i>=0;i--)
    delete nPipes[i];
  delete nPipes;
  //to IMPROVE
  system("killall modbus_client");
  return;
}
/*!function to check if protocol is already saved to configure*/
int CommDaemon::isDefined(int protocol)
{
  std::cout << "DEBUG: (inside CommDaemon::isDefined) " << std::endl;
  int defined = 0;
  if(!daemons.empty())
    {
      for(int i=0;i < daemons.size();i++)
	if(protocol == daemons.at(i))
	  defined = 1;
    }
  return defined;
}
/*!function for setting ini files for daemons, thanks to internal copy of slaves parameters*/
int CommDaemon::iniDaemons()
{
  std::cout << "DEBUG: (inside CommDaemon::iniDaemons) " << std::endl;
  int failed = -1;
  
   for(int i=0;i < daemons.size();i++)
     {
       if(daemons.at(i) == PROT_MODBUS_TCP)
	 failed = iniMBTCP();
     }
   
  return failed;
}

/*!function for setting ini files for a Modbus TCP/IP daemon, thanks to internal copy of slaves parameters*/
int CommDaemon::iniMBTCP()
{
  std::cout << "DEBUG: (inside CommDaemon::iniMBTCP) " << std::endl;
  int failed = -1; 
  char * iniFile = NULL;
  IniConfigurator iniConfig;
   
  iniFile = new char[strlen("./comm/modbus_client/MBTCP.ini")+5];
  sprintf(iniFile,"./comm/modbus_client/MBTCP.ini"); 
  std::cout << "INFO: setting MODBUS TCP/IP client ini file..."<< std::endl; 
  failed = iniConfig.iniMBTCPCreate(iniFile,slavesP);

  delete iniFile;

  return failed;
}
/*!function for launching all defined daemons
TODO: we are not checking if daemons are already launched!*/
int CommDaemon::launchDaemons()
{
  //std::cout << "DEBUG: (inside CommDaemon::launchDaemons)" << std::endl;
  int failed = -1, result = 0;
  int protocol = 0;

  if(!daemons.empty())
    {
      for(int i = 0; i < daemons.size(); i++)
	{
	    pthread_t* slaveThread = declareThread(daemons.at(i));
   
	    if(daemons.at(i) == PROT_MODBUS_TCP)
	      {
		std::cout << "DEBUG:(inside CommDaemon::launchDaemon) creating thread for:" << daemons.at(i) << std::endl;
		result = pthread_create(slaveThread, NULL, launchMBUS , (void *) (intptr_t) daemons.at(i));
		failed = 0;
		if (result)
		  {
		    std::cout << "ERROR:(inside CommDaemon::launchDaemon) unable to create thread!:" << result << std::endl;
		    failed = -1;
		  }

	      }
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

int renameOldLog(int protocol,char** logPath)
{
  static char *file1,*file2 = NULL;
  int failed = -1;

  file1 = new char[ 40];
  file2 = new char[40];

  if ( protocol ==PROT_MODBUS_TCP)
    {
      sprintf(file1,"./log/MBUSTCP.log");
      sprintf(file2,"./log/MBUSTCP_old.log");
      rename(file1, file2);
      failed = 0;
    }
  else
    {
      failed = -1;
    }
	
  
  *logPath = file1;
  delete file2;
  return 0;
}

int setExecutable(int commId,char **executable)
{
  std::cout << "DEBUG:(inside CommDaemon::setExecutable)"<< std::endl;
  int failed = -1;
  static char *file1 = NULL;
  std::cout << "DEBUG:(inside CommDaemon::setExecutable) commId:" << commId << std::endl;
  if(commId == PROT_MODBUS_TCP)
    {

      file1 = new char[sizeof("./comm/modbus_client/modbus_client ./comm/modbus_client/") + 40];
      sprintf(file1,"./comm/modbus_client/modbus_client ./comm/modbus_client/MBTCP.ini");
      failed = 0;
    }
  *executable = file1;
  return failed;
}

void* launchMBUS(void * protocol)
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
  int type = (intptr_t) protocol;
  
  renameOldLog(type,&tmpString);
  fout = fopen(tmpString,"w");
  delete tmpString;

  if (!setExecutable(type, &tmpString))
    {
      std::shared_ptr<FILE> pipe(popen(tmpString, "r"), pclose);
      if (!pipe) throw std::runtime_error("popen() failed!");
      std::cout << "DEBUG:(inside CommDaemon::launchMBUS) executable was launched: " << tmpString << std::endl;
      delete tmpString;
      while (!feof(pipe.get()))
	{
	  if (fgets(buffer, 500, pipe.get()) != NULL)
	    {
	      time(&rawTime);
	      timeInfo = localtime(&rawTime);
	      strftime (timeBuffer,10,"%T: ",timeInfo);	      
	      fprintf(fout,"%s%s", timeBuffer,buffer);
	      lines++;
	      if(lines >= maxLines)
	  	{
	  	  rewind(fout);
	  	  lines = 0;
	  	}
	    }

	}
    }
  else
    std::cout << "DEBUG:(inside CommDaemon::launchMBUS) executable was launched and has finished: " << tmpString << std::endl;
  pthread_exit(NULL);
  fclose(fout);
}
