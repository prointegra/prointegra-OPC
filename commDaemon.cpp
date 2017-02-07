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
@file commDaemon.cpp
'''launching,checking,logging... communication daemon manager class'''
*/

#include "commDaemon.h"

/*! constructor, creates pipes for every slave*/
CommDaemon::CommDaemon(int slaves)
{
  nSlaves = slaves;

  nPipes = new int*[nSlaves];
  for(int i=0;i<slaves;i++)
    nPipes[i] = new int[2];

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

int CommDaemon::launchDaemon(int nSlave, int commId, char * protocol)
{
  std::cout << "DEBUG: (inside CommDaemon::launchDaemon)" << std::endl;
  int failed = -1;
  int     result = 0;

  pthread_attr_t attr;
  pthread_t* slaveThread = declareThread(nSlave);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  if(!strcmp(protocol,"MODBUSTCP"))
    {
      result = pthread_create(slaveThread, &attr, launchMBUS , (void *)commId);
      failed = 0;
    }  
  if (result)
    {
      std::cout << "ERROR:(inside CommDaemon::launchDaemon) unable to create thread!:" << result << std::endl;
      failed = -1;
    }

  pthread_attr_destroy(&attr);
  
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

void* CommDaemon::launchMBUS(void * commId)
{
  char buffer[500];
  std::string result;
  char *tmpString = NULL;
  FILE *fout; // out file

  char timeBuffer[10];
  time_t rawTime;
  struct tm * timeInfo;
  int lines = 0;
  const int maxLines = 20000;
  int id = (long)commId;

  renameOldLog(id,&tmpString);
  fout = fopen(tmpString,"w");
  delete tmpString;
  if (!setExecutable(id,"MODBUSTCP", &tmpString))
    {
      std::shared_ptr<FILE> pipe(popen(tmpString, "w"), pclose);
      if (!pipe) throw std::runtime_error("popen() failed!");
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
  fclose(fout);
  pthread_exit(NULL);
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

int renameOldLog(int commId,char** logPath)
{
  char *file1,*file2 = NULL;

  file1 = new char[sizeof(char)*commId + 20];
  file2 = new char[sizeof(char)*commId + 30];

  sprintf(file1,"./log/id%d_rl.log",commId);
  sprintf(file2,"./log/id%d_rl_old.log",commId);
  
  rename(file1, file2);

  *logPath = file1;
  delete file2;
  return 0;
}

int setExecutable(int commId,char* protocol,char **executable)
{
  int failed = -1;
  char *file1 = NULL;

  if(!strcmp(protocol,"MODBUSTCP"))
    {

      file1 = new char[sizeof("./comm/modbus_client/modbus_client ./comm/modbus_client/") + commId*sizeof(char) +5];
      sprintf(file1,"./comm/modbus_client/modbus_client ./comm/modbus_client/id%d.ini &",commId);
      failed = 0;
    }
  *executable = file1;
  return failed;
}
