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
int IniConfigurator::iniMBTCPCreate(char *iniFile, mbSlaves & params)
{
  std::cout << "DEBUG: (inside IniConfigurator::iniMBTCPCreate) "<< std::endl;
  FILE * pFile;
  int ret = -1;

  pFile = fopen (iniFile,"w");

  if(pFile!=NULL)
    {
      ret = 0;
      fprintf(pFile,"#Config file created by Prointegra-OPC\n");
      fprintf(pFile,"target=%s\n",params.slaveName);
      fprintf(pFile,"shared_memory=./comm/%s.shm\n",params.slaveName);
      fprintf(pFile,"mailbox=./comm/%s.mbx\n",params.slaveName);
      fprintf(pFile,"#communication=serial\n");
      fprintf(pFile,"tty=/dev/ttyS0\n");
      fprintf(pFile,"baudrate=9600\n");
      fprintf(pFile,"rtscts=1\n");
      fprintf(pFile,"parity=0\n");
      fprintf(pFile,"protocol=0\n");
      fprintf(pFile,"communication=socket\n",1);
      fprintf(pFile,"tcpadr=%s\n",params.commAddr);
      fprintf(pFile,"tcpport=%d\n",params.commPort);      
      fprintf(pFile,"\n");
      
      writMBTCPCycles(pFile,params);
  
      fclose(pFile);
    }
  std::cout << "DEBUG: ini file finished!" << std::endl;
  return ret;
}

/*! function check if communications are made by socket
*/
int IniConfigurator::usingSocket(mbSlaves& parameters)
{
  int ret = 0;
  if(!strcmp(parameters.commType,"MODBUSTCP"))
     ret = 1;

  return ret;
}

/*!function for writting modbus TCP/IP socket section
TODO: port stuck to 502!*/
int IniConfigurator::writMBTCPSocket(FILE* pFile, mbSlaves params)
{
  int j = 1, failed = -1;
  fprintf(pFile,"[SOCKET]\n");
  fprintf(pFile,"PORT=%s\n",params.port);
  fprintf(pFile,"IP=%s\n",params.commAddr);
  fprintf(pFile,"\n");
  return failed;
}

/*! function for writting communication cycles in MODBUS TCP/IP ini file

*/
int IniConfigurator::writMBTCPCycles(FILE* pFile, mbSlaves& params)
{
  int failed=-1;
  std::vector <std::vector <int> > cyclesPerSlave;
  std::vector <int > definedCycle;
  int cyclePosition = 0;
  int baseCycle = 0;
    
  if(!strcmp(params.commType,"MODBUSTCP"))
    {
      //every MODBUS TCP/IP slave
      cyclesPerSlave.clear();
      definedCycle.clear();
      for(int j=0; j < params.nRegs ; j++)
	{
	  //every register
	  if(!definedCycle.empty())
	    {
	      if(definedCycle.back() == (params.stRegisters[j].iAddress-1)) //same cycle
		{
		  definedCycle.push_back(params.stRegisters[j].iAddress);
		  params.stRegisters[j].cycleBase = baseCycle;
		  params.stRegisters[j].cyclePosition = cyclePosition;
		}
	      else //saving cycle, creating new one
		{
		  cyclesPerSlave.push_back(definedCycle);
		  definedCycle.clear();
		  baseCycle = baseCycle + cyclePosition*2;
		  cyclePosition=0;
		  definedCycle.push_back(params.stRegisters[j].iAddress);
		  params.stRegisters[j].cycleBase = baseCycle;
		  params.stRegisters[j].cyclePosition = cyclePosition;  
		}		    
	    }
	  else
	    {
	      definedCycle.push_back(params.stRegisters[j].iAddress);
	      params.stRegisters[j].cycleBase = baseCycle;
	      params.stRegisters[j].cyclePosition = cyclePosition;
	    }
	  cyclePosition++;
	}
      cyclesPerSlave.push_back(definedCycle);	  
    }    
  for(int j=0; j < cyclesPerSlave.size() ; j++)
    {
      fprintf(pFile,"cycle%d slave=1 function=3 start_adr=%d num_register=%d\n",j+1,cyclesPerSlave.at(j).at(0),cyclesPerSlave.at(j).size());
      failed = 0;
    }

  return failed;
}

/*! function check if communications are made by socket for writting communication cycles
*/
int IniConfigurator::writCycles(FILE* pFile, mbSlaves* parameters)
{
  int cycles=0;
  int i=0, j=0;
  int failed=-1;
  int valid=0;
  int *first = new int(0);

  //INT TAGS
  //  while(tagTypeValid(parameters->stRegisters[i].dataType) && parameters->stRegisters[i].isValid)
  //  {
  //    i++;
  //  }
  if(parameters->nRegs>0)
    {
      cycles = rudeCycles(parameters,first);
      cycles = fineCycles(parameters,first,cycles);
      fprintf(pFile,"[CYCLES]\n");      
      fprintf(pFile,"NUM_CYCLES=%d\n",cycles);
      for(i=0;i < cycles;i++) 
	{
	  while(!valid)
	    {
	      if(cycleIsValid(parameters,(*first+j*50)))
		{
		  fprintf(pFile,"CYCLE%d=50,holdingRegisters(%d,%d)\n",i+1,parameters->commId,*first+j*50);
		  valid = 1;
		  j++;
		}
	      else
		  j++;

	    }
	  valid = 0;
	}
      failed = 0;
    }

  delete first;
  return failed;
}


  
/*! function to rude modbus cycle calculation
*/
int IniConfigurator::rudeCycles(mbSlaves* parameters, int * first)
{
  int min=0,max=0,cycles=0,i=0;
  min = max = parameters->stRegisters[0].iAddress;
  //rude cycles Calc
  for(i=0; i<parameters->nRegs;i++)
    {
      if((min > parameters->stRegisters[i].iAddress) && (parameters->stRegisters[i].isValid))
	min = parameters->stRegisters[i].iAddress;
      if((max < parameters->stRegisters[i].iAddress) && (parameters->stRegisters[i].isValid))
	max = parameters->stRegisters[i].iAddress;
    }
  cycles = (max - min) / 50 + 1;

  *first = min;
  return cycles;
}

/*! function to fine modbus cycle calculation
*/
int IniConfigurator::fineCycles(mbSlaves* parameters, int * first, int rudeCycles)
{
  int i=0,fineCycles = 0,j=0;
  int valid = 0;

  //fine cycles Calc
  for(i=0; i < rudeCycles;i++)
    {
      valid = 0;
      for(j=0; j <  parameters->nRegs ; j++)
	{
	  if(parameters->stRegisters[j].iAddress >= (*first + i*50) && parameters->stRegisters[j].iAddress < (*first + i*50 + 50))
	    valid = 1;
	}
      if (valid)
	  fineCycles++;

    }

  return fineCycles;
}

/*! function to check if a cycle of 50 registers from address is useful
*/
int IniConfigurator::cycleIsValid(mbSlaves* parameters, int address)
{
  int i=0;
  int valid = 0;


  for(i=0; i <  parameters->nRegs ; i++)
    {
      if(parameters->stRegisters[i].iAddress >= address && parameters->stRegisters[i].iAddress < address+50)
	valid = 1;
    }

  return valid;
}    
/*! function to check if a tag type is valid
*/
int IniConfigurator::tagTypeValid(const char * type)
{
  int valid = 0;

  if(!strcmp(type,"INT"))
    valid++;
  else if(!strcmp(type,"WORD"))
    valid++;

  return valid;
}     

