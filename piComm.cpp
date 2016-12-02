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
  IniConfigurator iniConfig;
  
  std::cout << "DEBUG: setting MODBUS TCP/IP rlDataAcquisition class..."<< std::endl;
  
  mailbox = new char[strlen("./comm/id.mbx")+sizeof(char)*10];
  sharedMemory = new char[strlen("./comm/id.shm")+sizeof(char)*10];
  iniFile = new char[strlen("./comm/modbus_client/id.ini")+sizeof(char)*10];
  
  sprintf(mailbox,"./comm/id%d.mbx",parameters.commId);
  sprintf(sharedMemory,"./comm/id%d.shm",parameters.commId);
  sprintf(iniFile,"./comm/modbus_client/id%d.ini",parameters.commId);
  
  rlMODBUS = new rlDataAcquisition(mailbox,sharedMemory, 65536);
  std::cout << "DEBUG: setting MODBUS TCP/IP client ini file..."<< std::endl;
 
  failed = iniConfig.iniCreate(iniFile,&parameters);
  if (failed)
    std::cout << "DEBUG:something has gone wrong configuring rlDataAcquistion structure!\n" << std::endl;

  delete iniFile;
  delete mailbox;
  delete sharedMemory;
  return 0;
}
/*!function to read all data */
int CommInterface::readData()
{
  int failed = -1;

  for(int i=0; i < parameters.nRegs;i++)
    {

      if(parameters.stRegisters[i].isValid)
	readTag(i);
      std::cout << "DEBUG: reading tag: " <<  parameters.stRegisters[i].tagName << ", int value: " << parameters.stRegisters[i].iValue << std::endl;
    }
  failed  = 0;

  return failed;
}
/*function to read a slave's tag*/
int CommInterface::readTag(int index)
{
  if(parameters.stRegisters[index].dataType == INT)
    parameters.stRegisters[index].iValue = readINT(index);

  return 0;
}
/*function to read a slave's int tag
TODO: for now only from holdingRegisters!*/
int CommInterface::readINT(int index)
{
  char *rlCommand;

  rlCommand = new char[sizeof("holdingRegisters(,)") + sizeof(parameters.commId) + sizeof(parameters.stRegisters[index].iAddress) + 5];
  sprintf(rlCommand,"holdingRegisters(%d,%d)",parameters.commId,parameters.stRegisters[index].iAddress);

  parameters.stRegisters[index].iValue = gstWord2Int(rlMODBUS->intValue(rlCommand));

  return 0;
}
