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
*/

#include "commDaemon.h"


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
  
  return;
}

int CommDaemon::launchDaemon(int nSlave, int commId, char * protocol)
{
  int failed = -1;
  pid_t pid;
  int     result;

  result = pipe (nPipes[nSlave]);
  if (result >= 0)
    {
      pid = fork();
      if (pid >= 0)
	{
	  if(pid == 0)
	    {
	      if(!strcmp(protocol,"MODBUSTCP"))
		 launchMODBUSDaemon(nSlave,commId);
	      failed = 0;
	    }
	  else
	    failed = 0;
	      	  
	}
      else //failure in creating a child
	perror ("fork");

    }
  else//failure in creating a pipe
     perror("pipe");


  
  return failed;
}

int CommDaemon::checkDaemon(int slave)
{
  int failed = -1;
  char message[4];
  
  read (nPipes[slave][0], message, sizeof(message));
  message[3] = '\0';
  //std::cout << "DEBUG: message string from daemon: " << message << std::endl;
  if(strstr(message,"1")!=NULL)
    failed = 0;

  return failed;
}
  
int CommDaemon::launchMODBUSDaemon(int nSlave, int commId)
{
  int failed = -1;
  char *tmpString = NULL;
  char line[128];
  FILE *file;
  FILE *fout; // out file

  renameOldLog(commId,&tmpString);
  fout = fopen(tmpString,"w");
  delete tmpString;
  if (!setExecutable(commId,"MODBUSTCP", &tmpString))
    {
  
      file = popen(tmpString, "r");
      if (!file) 
	{ 
	  fprintf(stderr,"ERROR: bad command to execute modbus tcp ip daemon\n");
	  perror(".daemon_log"); 
	  return -1;        
	}
      //show output
      while (fgets(line, 128, file) != NULL) //buffer can be improved IMPROVE
	{
	  fprintf(fout,"%s", line);
	    write(nPipes[nSlave][1], "11", strlen("nn"));
	}
      write(nPipes[nSlave][1], "00", strlen("nn"));
      pclose(file);
      delete tmpString;
    }
  else
    {
      fprintf(stderr,"ERROR: bad command to execute modbus tcp ip daemon\n");
    }
  pclose(fout);

  
  return failed;
}

////tools

int CommDaemon::renameOldLog(int commId,char** logPath)
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

int CommDaemon::setExecutable(int commId,char* protocol,char **executable)
{
  int failed = -1;
  char *file1 = NULL;

  if(!strcmp(protocol,"MODBUSTCP"))
    {

      file1 = new char[sizeof("./comm/modbus_client/modbus_client ./comm/modbus_client/") + commId*sizeof(char)];
      sprintf(file1,"./comm/modbus_client/modbus_client ./comm/modbus_client/id%d.ini",commId);
      failed = 0;
    }
  *executable = file1;
  return failed;
}
