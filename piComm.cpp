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
  int failed = -1;
  //taking db parameters
  parameters = slaveParams;
  if(parameters.isValid)
    {
      std::cout << "DEBUG: configuring communications for slave with id = " << parameters.commId << std::endl;
      //setting communication daemon
      failed = setupCommDaemon();    
    }
  return failed;
}

/*! function to setup ini file, mailbox,shared memory and instance of rlib communications */
int CommInterface::setupCommDaemon()
{
  int failed = -1;
  if(!strcmp(parameters.commType,"MODBUSTCP"))
    {
      std::cout << "INFO: MODBUS TCP/IP found, configuring..."<< std::endl;
      failed = setupMBUSTCP();
    }
  return failed;
}

/*! function to setup ini file, mailbox,shared memory and instance of MODBUS TCP/IP rlib communications */
int CommInterface::setupMBUSTCP()
{
  int failed = -1;
  char *mailbox = NULL;
  char *sharedMemory = NULL;
  char * iniFile = NULL;
  IniConfigurator iniConfig;
  
  //std::cout << "DEBUG: setting MODBUS TCP/IP rlDataAcquisition class..."<< std::endl;
  
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
  return failed;
}
/*!function to read all data 
TODO: error returning?*/
int CommInterface::readData()
{
  int failed = -1;

  failed = 0;
  for(int i=0; i < parameters.nRegs;i++)
    {     
      failed = failed & readTag(i);
    }

  return failed;
}
/*function to read a slave's tag*/
int CommInterface::readTag(int index)
{
  int ret = -1;
  if(!strcmp(parameters.stRegisters[index].dataType,"INT"))
    {
      if(readINT(index))
	parameters.stRegisters[index].valueValid = 0;
      else
	parameters.stRegisters[index].valueValid = 1;
    }
  else if (!strcmp(parameters.stRegisters[index].dataType,"WORD"))
    {
      if(readWORD(index))
	parameters.stRegisters[index].valueValid = 0;
      else
	parameters.stRegisters[index].valueValid = 1;
    }
  else
    parameters.stRegisters[index].valueValid = 0;
  if(parameters.stRegisters[index].valueValid)
    ret = 0;
  return ret;
}
/*function to read a slave's int tag
TODO: for now only from holdingRegisters!*/
int CommInterface::readINT(int index)
{
  char *rlCommand;
  int failed = DAQ_ERROR;

  rlCommand = new char[sizeof("holdingRegisters(,)") + sizeof(parameters.commId) + sizeof(parameters.stRegisters[index].iAddress) + 5];
  sprintf(rlCommand,"holdingRegisters(%d,%d)",parameters.commId,parameters.stRegisters[index].iAddress);
  //std::cout <<"DEBUG: reading tag, command:"<< rlCommand <<" raw data:"<<rlMODBUS->intValue(rlCommand)<<" int data:"<< gstWord2Int(rlMODBUS->intValue(rlCommand)) << std::endl;
  if (rlMODBUS->intValue(rlCommand) != DAQ_ERROR)
    {
      failed = 0;
      parameters.stRegisters[index].iValue = gstWord2Int(rlMODBUS->intValue(rlCommand));
    }

  return failed;
}

/*function to read a slave's word tag
TODO: for now only from holdingRegisters!*/
int CommInterface::readWORD(int index)
{
  char *rlCommand;
  int failed = DAQ_ERROR;

  rlCommand = new char[sizeof("holdingRegisters(,)") + sizeof(parameters.commId) + sizeof(parameters.stRegisters[index].iAddress) + 5];
  sprintf(rlCommand,"holdingRegisters(%d,%d)",parameters.commId,parameters.stRegisters[index].iAddress);
  //std::cout <<"DEBUG: reading tag, command:"<< rlCommand <<" raw data:"<<rlMODBUS->intValue(rlCommand)<<" word data:"<< rlMODBUS->intValue(rlCommand) << std::endl;
  if (rlMODBUS->intValue(rlCommand) != DAQ_ERROR)
    {
      failed = 0;
      parameters.stRegisters[index].iValue = rlMODBUS->intValue(rlCommand);
    }
  return failed;
}
//
//WRITTING FUNCTIONS
/*! function overloaded for writting int/word tag value*/
int CommInterface::writeTag(int index, int slaveAmI, int value)
{
  //std::cout << "DEBUG: (inside CommInterface::writeTag)" << std::endl;
  int failed = -1;
  int iValue = value;
  char rlCommand[100];

  if(index < parameters.nRegs)
    {
      if(!strcmp(parameters.stRegisters[index].dataType,"INT"))
	iValue = gstInt2Word(value);
      sprintf(rlCommand,"register(%d,%d)",slaveAmI, parameters.stRegisters[index].iAddress);
      //write!
      //std::cout << "DEBUG: (inside CommInterface::writeTag) writting sequence: " << rlCommand << "  value = " << iValue << std::endl;
      failed = rlMODBUS->writeIntValue(rlCommand,iValue);
      //std::cout << "DEBUG: (inside CommInterface::writeTag) writting sequencemodbus ret: " << failed << std::endl;
      failed = 0;
    }
  
  return 0;
}
/*!Function for returning a tag name
TODO: it shoulds return only once*/
char* CommInterface::retTagName(int tag)
{
  if(tag >=0 && tag < parameters.nRegs)
    return parameters.stRegisters[tag].tagName;
  else
    return NULL;

}

/*!Function for returning a tag int value
TODO: MORE TYPES, strings, float, bool, etc.*/
int CommInterface::retTagValue(int tag)
{
  int ret = 0;
  if(tag >=0 && tag < parameters.nRegs)
    ret = parameters.stRegisters[tag].iValue;
  return ret;
}

/*!Function for returning if a tag value is valid
TODO: it shoulds return only once*/
int CommInterface::retTagValid(int tag)
{
  int ret = 0;
  if(tag >=0 && tag < parameters.nRegs)
    ret = parameters.stRegisters[tag].valueValid;
  return ret;

}
