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
      std::cout << "DEBUG: configuring communications for slave with id = " << parameters.commId << std::endl;
      //setting communication daemon
      setupCommDaemon();

      
    }

  return 0;
}

/*! function to setup ini file, mailbox,shared memory and instance of rlib communications */
int CommInterface::setupCommDaemon()
{
  int failed = -1;
  if(!strcmp(parameters.commType,"MODBUSTCP"))
    {
      std::cout << "DEBUG: MODBUS TCP/IP found, configuring..."<< std::endl;
      failed = setupMBUSTCP();
    }
  else
    failed = -1;
  return 0;
}

/*! function to setup ini file, mailbox,shared memory and instance of MODBUS TCP/IP rlib communications */
int CommInterface::setupMBUSTCP()
{
  int failed = -1;
  char *mailbox = NULL;
  char *sharedMemory = NULL;
  char * iniFile = NULL;
  
  std::cout << "DEBUG: setting MODBUS TCP/IP rlDataAcquisition class..."<< std::endl;
  
  mailbox = new char[strlen("./comm/id.mbx")+sizeof(char)*10];
  sharedMemory = new char[strlen("./comm/id.shm")+sizeof(char)*10];
  iniFile = new char[strlen("./comm/modbus_client/id.ini")+sizeof(char)*10];
  
  sprintf(mailbox,"./comm/id%d.mbx",parameters.commId);
  sprintf(sharedMemory,"./comm/id%d.shm",parameters.commId);
  sprintf(iniFile,"./comm/modbus_client/id%d.ini",parameters.commId);
  
  rlMODBUS = new rlDataAcquisition(mailbox,sharedMemory, 65536);
  std::cout << "DEBUG: setting MODBUS TCP/IP client ini file..."<< std::endl;
  failed = iniSet(iniFile);
  if (failed)
    std::cout << "DEBUG:something has gone wrong configuring rlDataAcquistion structure!\n" << std::endl;

  delete iniFile;
  delete mailbox;
  delete sharedMemory;
  return 0;
}

/*! function to setup ini file, [global] paragraph  
//TODO:manually created for modbus TCP/IP
it should be extern to piComm, another class?
we give it parameters and it builds dinamically an ini file
*/
int CommInterface::iniSet(char *iniFile)
{
  FILE * pFile;
  int ret = -1;
  int min,max=0;

  pFile = fopen (iniFile,"w");

  if(pFile!=NULL)
    {
      ret = 0;
      fprintf(pFile,"//Config file created by Prointegra-OPC\n");
      fprintf(pFile,"\n");
      fprintf(pFile,"[GLOBAL]\n");
      fprintf(pFile,"USE_SOCKET=1\n");
      fprintf(pFile,"DEBUG=1\n");
      fprintf(pFile,"CYCLETIME=1000\n");
      fprintf(pFile,"N_POLL_SLAVE=10\n");      
      fprintf(pFile,"\n");   
      fprintf(pFile,"[SOCKET]\n");
      fprintf(pFile,"IP=%s\n",parameters.commAddr);
      fprintf(pFile,"PORT=%d\n",parameters.commPort);
      fprintf(pFile,"\n");   
      fprintf(pFile,"[RLLIB]\n");
      fprintf(pFile,"MAX_NAME_LENGTH=30\n");
      fprintf(pFile,"SHARED_MEMORY=../id%d.shm\n",parameters.commId);
      fprintf(pFile,"SHARED_MEMORY_SIZE=65536\n");
      fprintf(pFile,"MAILBOX=../id%d.mbx\n",parameters.commId);
      //cycles CALC?!?!?! TO DO!
      fprintf(pFile,"\n");
      fprintf(pFile,"[CYCLES]\n");      
      fprintf(pFile,"NUM_CYCLES=3\n");
      fprintf(pFile,"CYCLE1=50,holdingRegisters(%d,100)\n",parameters.commId);
      fprintf(pFile,"CYCLE2=50,holdingRegisters(%d,150)\n",parameters.commId);      
      fprintf(pFile,"CYCLE3=50,holdingRegisters(%d,200)\n",parameters.commId);

      fclose(pFile);
    }
  return ret;
}
