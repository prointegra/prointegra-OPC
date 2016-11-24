/*
 * 	Prointegra OPC
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
@File config.h
*/
#ifndef _CONFIG_
#define _CONFIG_
#include "pugixml.hpp"
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include "rldataacquisition.h"

using namespace std;

extern const char* sVERSION;

enum RLLIB_ERRORS
  {

    DAQ_ERROR = 8388608
  };


//members
/*!register to save
typedef struct 
{
  int index;
  char tag_name[12];
  int valueLL;
  int valueL;
  int valueH;
  int valueHH;
  char date[20];
  char time[20];
  int cycle;
  char comment[100];
  int type;
  int err;
  } mbRegData;*/

//members
/*!register trnasmitted*/
typedef struct 
{
  int Datatype;
  string address;
  string addressH;
  string addressHH;
  string addressHHH;
} mbReadData;

//members
/*!slaves to communicate with*/
typedef struct 
{
  int slaveId;
  int commType; //1->modbus RTU, 2-> modbus TCP/IP
  int mTime;
  mbReadData* Registers;
  int nRegs;
} mbSlaves;


//database parameters
typedef struct
{
  char *type = NULL;
  char *host = NULL;
  char *dbName = NULL;
  char *internalName = NULL;
  char *user = NULL;
  char *pass = NULL;
  int numTables = 0;
} databaseParameters;

//table parameters
typedef struct
{
  char * tbName = NULL;
  char * tbTrigger = NULL;
  int tbTriggerTime;
  int numFields;
  char*** field; //field name, field type, field var
  
} tableParameters;

class configParser 
{
 public:
  configParser (char *);
  ~configParser ();  

  //database Parsing
  int retrieveDBParams();
  int retrieveCharAttr(pugi::xml_node* db, char** name, const char* attribute);
  int retrieveIntAttr(pugi::xml_node* db, int* value, const char* attribute);
  int retrieveTableAttrs(pugi::xml_node* db, int dbNumber, int numTables);
  //check fucntions
  int checkDBParams(int i);
  int checkDBType(const char* type);
  //private members returning functions
  int retnDBs(){ return nDBs;}
  databaseParameters retDBParams(int database);

 private:
  int retrieveNumberofNodes(pugi::xml_document* master , const char* concept);
  int retrieveNumberofNodes(pugi::xml_node* master , const char* concept);
  /*
  int IterateComms();
  int RetrieveConfig();
  int getnumSlaves(){return nSlaves;}
  int getSlaveId(int id);
  int getSlaveMailbox(int id,rlDataAcquisition **);
  int getSlavemTime(int id);
  int getSlavenRegs(int id); 
  int getSlaveReadData(int id,mbReadData **);
  //Communications
  int setModbusRTU(int i){MODBUSRTU = i; return 0;}
  int setModbusTCP(int i){MODBUSTCP = i; return 0;}
  int getModbusRTU(){return MODBUSRTU;}
  int getModbusTCP(){return MODBUSTCP;}
*/
 private:
  pugi::xml_document doc;
  //databases parameters
  databaseParameters* databaseParams;
  tableParameters** tablesParams;
  int nDBs;
  //number of slaves and type
  mbSlaves* ConfigSlaves;
  int nSlaves;

  int MODBUSRTU;
  int MODBUSTCP;

  const char* stypes[2] = {"INT","MOD10"};
  const int itypes[2]= {1,10};
  const int ntypes = 2;
};




#endif
