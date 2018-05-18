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
int IniConfigurator::iniMBTCPCreate(char *iniFile, std::vector <mbSlaves> params)
{
  std::cout << "DEBUG: (inside IniConfigurator::iniMBTCPCreate) "<< std::endl;
  FILE * pFile;
  int ret = -1;

  pFile = fopen (iniFile,"w");

  if(pFile!=NULL)
    {
      ret = 0;
      fprintf(pFile,"#Config file created by Prointegra-OPC\n");
      fprintf(pFile,"\n");
      fprintf(pFile,"[GLOBAL]\n");
      fprintf(pFile,"USE_SOCKET=%d\n",1);
      //by default (TO IMPROVE?)
      fprintf(pFile,"DEBUG=0\n");
      fprintf(pFile,"CYCLETIME=500\n");
      fprintf(pFile,"N_POLL_SLAVE=0\n");      
      fprintf(pFile,"\n");
      //logging capability //only available with modbus_client2 no 1
      fprintf(pFile,"[LOGGING]\n");
      fprintf(pFile,"LOG=\n");
      fprintf(pFile,"\n");
      
      writMBTCPSocket(pFile,params);

      fprintf(pFile,"[RLLIB]\n");
      fprintf(pFile,"MAX_NAME_LENGTH=30\n");
      fprintf(pFile,"SHARED_MEMORY=./comm/MBTCP.shm\n");
      fprintf(pFile,"SHARED_MEMORY_SIZE=65536\n");
      fprintf(pFile,"MAILBOX=./comm/MBTCP.mbx\n");
      fprintf(pFile,"\n");
      writMBTCPCycles(pFile,params);
  
      fclose(pFile);
    }
  std::cout << "DEBUG: ini file finished!" << std::endl;
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

/*!function for writting modbus TCP/IP socket section
TODO: port stuck to 502!*/
int IniConfigurator::writMBTCPSocket(FILE* pFile, std::vector <mbSlaves> params)
{
  int numSlaves = 0, j = 1, failed = -1;
  fprintf(pFile,"[SOCKET]\n");
  for(int i=0; i < params.size() ; i++)
    if(!strcmp(params.at(i).commType,"MODBUSTCP"))
      numSlaves++;
  fprintf(pFile,"NUM_SLAVES=%d\n",numSlaves);
  fprintf(pFile,"PORT=%d\n",502);
  for(int i=0; i < params.size() ; i++)
    {
      if(!strcmp(params.at(i).commType,"MODBUSTCP"))
	{
	  fprintf(pFile,"IP%d=%s\n",j,params.at(i).commAddr);
	  j++;
	  failed = 0;
	}
    }
  fprintf(pFile,"\n");
  return failed;
}

/*! function for writting communication cycles in MODBUS TCP/IP ini file
TODO: slaves.xml is supposed to be with tags well ordered by address!!
TODO: it makes error, cyles should not be grater than 90 registers, and there is no check at all
*/
int IniConfigurator::writMBTCPCycles(FILE* pFile, std::vector <mbSlaves> params)
{
  int totalCycles = 0;
  int cycleNumber = 1;
  int failed=-1;
  std::vector <std::vector <std::vector <int> > > cycles;
  std::vector <std::vector <int> > cyclesPerSlave;
  std::vector <int > definedCycle;
  

  fprintf(pFile,"[CYCLES]\n");
  cycles.clear();
  for(int i=0; i < params.size() ; i++)
    {
      if(!strcmp(params.at(i).commType,"MODBUSTCP"))
	{
	  //every MODBUS TCP/IP slave
	  cyclesPerSlave.clear();
	  definedCycle.clear();
	  for(int j=0; j < params.at(i).nRegs ; j++)
	    {
	      //every register
	      if(!definedCycle.empty())
		{
		  if(definedCycle.back() == (params.at(i).stRegisters[j].iAddress-1))
		    definedCycle.push_back(params.at(i).stRegisters[j].iAddress);
		  else //saving cycle, creating new one
		    {
		      cyclesPerSlave.push_back(definedCycle);
		      definedCycle.clear();
		      definedCycle.push_back(params.at(i).stRegisters[j].iAddress);
		    }
		    
		}
	      else
		definedCycle.push_back(params.at(i).stRegisters[j].iAddress);
	    }
	  cyclesPerSlave.push_back(definedCycle);	  
	  cycles.push_back(cyclesPerSlave);
	}
    }
  for(int i = 0; i < cycles.size();i++)
    totalCycles = totalCycles + cycles.at(i).size();
	
  fprintf(pFile,"NUM_CYCLES=%d\n",totalCycles);
  for(int i=0; i < cycles.size() ; i++)
    {
      for(int j=0; j < cycles.at(i).size() ; j++)
	{
	  fprintf(pFile,"CYCLE%d=%d,holdingRegisters(%d,%d)\n",cycleNumber,cycles.at(i).at(j).size(),i+1,cycles.at(i).at(j).at(0));
	  cycleNumber++;
	} 	
    }
  failed = 0;

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

