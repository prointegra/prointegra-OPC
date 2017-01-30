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
@File config.h
*/
#ifndef _CONFIG_
#define _CONFIG_
#include "pugixml.hpp"
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include "rldataacquisition.h"

using namespace std;

extern const char* sVERSION;

enum RLLIB_ERRORS
  {
    DAQ_ERROR = 8388608
  };

//members
/*!register trnasmitted*/
typedef struct 
{
  char* tagName = NULL;
  char * dataType = NULL;
  int iAddress = 0;
  int isValid = 0;

  int iValue = 0;
  int valueValid = 0;
} mbReadData;

//members
/*!slaves to communicate with*/
typedef struct 
{
  char *slaveName = NULL;
  char *commType = NULL;
  char *port  = NULL;
  int isValid = 0;

  int commId = -1;
  char* commAddr = NULL;
  int commPort = -1;
  
  int nRegs = 0;
  mbReadData* stRegisters = NULL;
} mbSlaves;

//field parameters
typedef struct
{
  char * name = NULL;
  char * type = NULL;
  char * tag = NULL;

  int isValid = 0;
  int iValue = 0;
  //trigger
  int isDone = 0;

  int fromSlave= -1;
  int fromTag= -1;
  std::vector<std::vector <int>> fromTags;
  
  //triggers
  int forRTable=-1;
  int forWTable=-1;
  
} field;

//database parameters
typedef struct
{
  int id = -1; //identification number
  
  int isValid = 0;
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
  int id = -1; //identification number
  
  int isValid;
  char * tbName; //table name
  
  char * tbTrigger; //table read trigger
  int tbTriggerTime;
  int RTrigger;
  
  char * tbWTrigger;
  int WTrigger;
  
  int numFields; //number of fields
  char * tbType; //table type
  field* stField;  //fields structures

} tableParameters;

class ConfigParser 
{
 public:
  ConfigParser (char *, char *);
  ~ConfigParser ();  

  ////database Parsing////
  int retrieveDBParams();
  //check functions
  int checkDBParams(int i);
  int checkTableParams(int db, int table);
  int checkDBType(const char* type); 
  //private members returning functions
  int retnDBs(){ return nDBs;}
  databaseParameters retDBParams(int database);
  tableParameters* retDBTables(int database);
  
  ////communications Parsing////
  int retrieveCommParams();
  
  //check functions
  int checkSlaveParams(int i);
  int checkSlaveName(const char * name, int index);
  int checkSlaveProtocol(const char* protocol);
  int checkSlaveId(int index);
  int checkTagData(int i);  
  //private members returning functions
  int retnSlaves(){ return nSlaves;}  
  mbSlaves retSlaveParams(int slave);
    
 private:
  int retrieveTablesParams(pugi::xml_node* db, int dbIndex, int numTables);
  int retrieveSlaveTags(pugi::xml_node* slave, int slaveIndex);
  int retrieveCharAttr(pugi::xml_node* db, char** name, const char* attribute);
  int retrieveIntAttr(pugi::xml_node* db, int* value, const char* attribute);
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
  pugi::xml_document commDoc;  
  //databases parameters
  databaseParameters* databaseParams;
  tableParameters** tablesParams;
  int nDBs;
  //number of slaves and type
  mbSlaves* slaveParams;
  mbReadData** allTags;
  int nSlaves;
};




#endif
