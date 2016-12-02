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
@File iniConfigurator.cpp
'''modbus_Client ini file creator'''
*/

#include "iniConfigurator.h"



/*! function to setup ini file, [global] paragraph  
//TODO:manually created for modbus TCP/IP
it should be extern to piComm, another class?
we give it parameters and it builds dinamically an ini file
it should be parted in smaller functions
*/
int IniConfigurator::iniCreate(char *iniFile, mbSlaves* parameters)
{
  FILE * pFile;
  int ret = -1;
  int socket = 0;
  int min,max=0;

  pFile = fopen (iniFile,"w");

  if(pFile!=NULL)
    {
      ret = 0;
      fprintf(pFile,"#Config file created by Prointegra-OPC\n");
      fprintf(pFile,"\n");
      fprintf(pFile,"[GLOBAL]\n");
      socket = usingSocket(parameters);
      fprintf(pFile,"USE_SOCKET=%d\n",socket);
      //by default (TO IMPROVE?)
      fprintf(pFile,"DEBUG=1\n");
      fprintf(pFile,"CYCLETIME=1000\n");
      fprintf(pFile,"N_POLL_SLAVE=10\n");      
      fprintf(pFile,"\n");
      if(socket)
	{
	  fprintf(pFile,"[SOCKET]\n");
	  fprintf(pFile,"IP=%s\n",parameters->commAddr);
	  fprintf(pFile,"PORT=%d\n",parameters->commPort);
	  fprintf(pFile,"\n");
	}
      else
	{
	  fprintf(pFile,"[TTY]\n");
	  fprintf(pFile,"DEVICENAME=%s\n",parameters->port);
	  std::cout << "ERROR: TTY comunication parameters not implemented" << std::endl;
	}
      fprintf(pFile,"[RLLIB]\n");
      fprintf(pFile,"MAX_NAME_LENGTH=30\n");
      fprintf(pFile,"SHARED_MEMORY=./comm/id%d.shm\n",parameters->commId);
      fprintf(pFile,"SHARED_MEMORY_SIZE=65536\n");
      fprintf(pFile,"MAILBOX=./comm/id%d.mbx\n",parameters->commId);
      //cycles CALC?!?!?! TO DO!
      fprintf(pFile,"\n");
      writCycles(pFile,parameters);
  
      fclose(pFile);
    }
  return ret;
}

/*! function check if communications are made by socket
*/
int IniConfigurator::usingSocket(mbSlaves* parameters)
{
  int ret = 0;
  if(!strcmp(parameters->commType,"MODBUSTCP"))
     ret = 1;

  return ret;
}
   
/*! function check if communications are made by socket
*/
int IniConfigurator::writCycles(FILE* pFile, mbSlaves* parameters)
{
  int cycles = 0;
  int min,max,i = 0;
  
  //INT TAGS
  while(parameters->stRegisters[i].dataType!=INT && parameters->stRegisters[i].isValid)
    {
      i++;
    }
  min = max = parameters->stRegisters[i].iAddress;
  
  for(i=0; i<parameters->nRegs;i++)
    {
      if((min > parameters->stRegisters[i].iAddress) && (parameters->stRegisters[i].isValid))
	min = parameters->stRegisters[i].iAddress;
      if((max < parameters->stRegisters[i].iAddress) && (parameters->stRegisters[i].isValid))
	max = parameters->stRegisters[i].iAddress;
    }
  cycles = (max - min) / 50 + 1;
  fprintf(pFile,"[CYCLES]\n");      
  fprintf(pFile,"NUM_CYCLES=%d\n",cycles);
  for(i=0;i < cycles;i++)
    {
      fprintf(pFile,"CYCLE%d=50,holdingRegisters(%d,%d)\n",i+1,parameters->commId,min+i*50);
    }

  return 0;
}     
