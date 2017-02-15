/*
 *  Prointegra OPC
 *
 *  Copyright 2016,2017 by it's authors. 
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
/**
@file config.c
*/

#include "config.h"
//constants
const char* sVERSION = "v0.0.31";

/*! Constructor
TODO:should we have to catch exceptions??¿*/
ConfigParser::ConfigParser(char* path, char *slaves)
{
  
  //initialization of files
  pugi::xml_parse_result result = doc.load_file(path); 
  std::cout << "INFO: database XML Load result: " << result.description()   << std::endl;
  result = commDoc.load_file(slaves); 
  std::cout << "INFO: communications XML Load result: " << result.description()   << std::endl;  
  return;   
}
/*! Destructor*/
ConfigParser::~ConfigParser()
{
  if (databaseParams != NULL)
    delete databaseParams;
       
  return;   
}
////database functions////
/*!retrieving database parameters*/
int ConfigParser::retrieveDBParams()
{
  int i=0;
  int nTables = 0;
  int size;
  int saveMode;
 
  //number of DB instances defined
  nDBs = retrieveNumberofNodes(&doc,"db");
  databaseParams = new databaseParameters[nDBs];
  tablesParams = new tableParameters*[nDBs];
  i = 0;
  //capturing data from databases;
  for (pugi::xml_node db = doc.child("db"); db; db = db.next_sibling("db"))
    {
      //retrieve database name (name of whole entity, database name + hostname +user + password + qtdriver + number of tables...)
      retrieveCharAttr(&db,databaseParams[i].internalName,"name");
      //retrieve database qt driver
      retrieveCharAttr(&db,databaseParams[i].type,"qtdriver");
      //retrieve database hostname
      retrieveCharAttr(&db,databaseParams[i].host,"hostname");
      //retrieve database id name
      retrieveCharAttr(&db,databaseParams[i].dbName,"dbName");    
      //retrieve database user
      retrieveCharAttr(&db,databaseParams[i].user,"user");
      //retrieve database id name user password
      retrieveCharAttr(&db,databaseParams[i].pass,"password");
      //number of tables
      databaseParams[i].numTables = retrieveNumberofNodes(&db,"table");
      //is everything ok? 
      if(!checkDBParams(i))
	databaseParams[i].isValid = 1;
      else
	databaseParams[i].isValid = 0;
      //
      std::cout << "INFO: processing tables from "<< databaseParams[i].internalName <<" database..." << std::endl;
      retrieveTablesParams(&db,i,databaseParams[i].numTables);
      i++;      
    }
  //i = number of databases created 
  return 0;
}

/*!function to check Database parameters integrity 
TODO: to improve the check!
*/
int ConfigParser::checkDBParams(int i)
{
  int failed = -1;
  if (databaseParams[i].internalName != NULL)
    {
      if(!checkDBType(databaseParams[i].type))
	{
	  failed = 0;		    
	}
    }
  return failed;
}

/*!function to check table parameters integrity 
TODO: to improve the check!
*/
int ConfigParser::checkTableParams(int db, int table)
{
  int failed = -1;

  //WORKAROUND: check must go here
  failed = 0;
  
  return failed;
}

/*!function to check tableDatabase type 
TODO: we only check if driver is either QSQLITE or QMYSQL
*/
int ConfigParser::checkDBType(const char* type)
{
  char * temp = NULL;
  int failed = -1;

  temp = new char[strlen(type)+1];
  strcpy(temp,type);
  for(int i=0;i<strlen(temp);i++)
    temp[i] = toupper(temp[i]);
  if(!strcmp(temp,"QSQLITE") || !strcmp(temp,"QMYSQL"))
    {
      failed = 0;
    }
  if(failed)
    cout << "ERROR!: DATABASE TYPE " << temp << " DOESN'T EXIST OR IS NOT IMPLEMENTED" << endl;
  delete temp;
  return failed;
}

/*!function for returning a database parameters instance
TODO:fucntions must have only one return point!*/
databaseParameters ConfigParser::retDBParams(int database)
{
  databaseParameters temp;
  
  if(database < nDBs && database >= 0)
    {
      return databaseParams[database];
    }
  else
    return temp;
}

/*!function for returning a table parameters instance array from a given database (by index)
TODO:fucntions must have only one return point!*/
tableParameters* ConfigParser::retDBTables(int database)
{
  tableParameters* temp = new tableParameters[0];
  if(database < nDBs && database >= 0)
    {
      return tablesParams[database];
    }
  return temp;
}

/*! function to retrieve all data from table in database, and creating it in memory struct */
int ConfigParser::retrieveTablesParams(pugi::xml_node* db, int dbNumber, int numTables)
{

  char* fieldName = NULL;
  char* fieldTagName = NULL;
  char* fieldType= NULL;
  int i;

  //defining number of tables in db
  i = numTables;
  std::cout << "INFO: " << numTables << " tables found..." << std::endl;
  tablesParams[dbNumber] = new tableParameters[i]{};
  //retrieving table parameters
  i=0;
  for (pugi::xml_node table = db->child("table"); table; table = table.next_sibling("table"))
    {
      retrieveCharAttr(&table,tablesParams[dbNumber][i].tbName,"name");
      retrieveCharAttr(&table,tablesParams[dbNumber][i].tbTrigger,"tagReadTrigger");
      retrieveIntAttr(&table,&tablesParams[dbNumber][i].tbTriggerTime,"timeReadTrigger");
      retrieveCharAttr(&table,tablesParams[dbNumber][i].tbWTrigger,"tagWriteTrigger");
      retrieveCharAttr(&table,tablesParams[dbNumber][i].tbType,"type");
      //tags
      tablesParams[dbNumber][i].numFields = retrieveNumberofNodes(&table,"tag");
      
      tablesParams[dbNumber][i].stField = new field[tablesParams[dbNumber][i].numFields];
      int k =0;
      for (pugi::xml_node tag = table.child("tag"); tag; tag = tag.next_sibling("tag"))
	{
	  retrieveCharAttr(&tag,tablesParams[dbNumber][i].stField[k].name,"name");
       	  retrieveCharAttr(&tag,tablesParams[dbNumber][i].stField[k].tag,"tagName");
	  retrieveCharAttr(&tag,tablesParams[dbNumber][i].stField[k].type,"type");
	  
	  std::cout << "INFO: tag = " << k+1 << " from table " << i+1 <<" processed" << std::endl;
	  
	  k++;
	}
      
      if(!checkTableParams(dbNumber,i))
	tablesParams[dbNumber][i].isValid = 1;
      else
	tablesParams[dbNumber][i].isValid = 0;		  
      i++;
    } 
   
  return 0;
}
////Communication functions////
/*!retrieving slaves parameters*/
int ConfigParser::retrieveCommParams()
{
  int i=0;
 
  //number of slaves instances defined
  nSlaves = retrieveNumberofNodes(&commDoc,"slave");
  slaveParams = new mbSlaves[nSlaves];
  
  i = 0;
  //capturing data from databasesslaves;
  for (pugi::xml_node slave = commDoc.child("slave"); slave; slave = slave.next_sibling("slave"))
    {
      //retrieve slave name
      retrieveCharAttr(&slave,slaveParams[i].slaveName,"name");
      //retrieve communications type
      retrieveCharAttr(&slave,slaveParams[i].commType,"protocol");
      //retrieve PC port
      retrieveCharAttr(&slave,slaveParams[i].port,"port");
      if (pugi::xml_node protocol = slave.child(slaveParams[i].commType))
	{
	  //retrieve id if any
	  retrieveIntAttr(&protocol,&slaveParams[i].commId,"id");  
	  //retrieve address if any
	  retrieveCharAttr(&protocol,slaveParams[i].commAddr,"addr");
	  //retrieve port if any
	  retrieveIntAttr(&protocol,&slaveParams[i].commPort,"port");
	}
	
      if(!checkSlaveParams(i))
	{
	  slaveParams[i].isValid = 1;
	}
      else
	slaveParams[i].isValid = 0;

      std::cout << "INFO: processing tags from "<< slaveParams[i].slaveName <<" slave, with id = " << slaveParams[i].commId << " ..." << std::endl;
      retrieveSlaveTags(&slave,i);
      i++;      
    }
  return 0;
}

/*! function to retrieve all data from table in database, and creating it in memory struct */
int ConfigParser::retrieveSlaveTags(pugi::xml_node* slave, int index)
{
  int i;

  //defining number of tags in slave protocol
  if (pugi::xml_node protocol = slave->child( slaveParams[index].commType))
    {
      slaveParams[index].nRegs = retrieveNumberofNodes(&protocol,"tag");
      std::cout << "INFO: "<< slaveParams[index].nRegs << " tags found..." << std::endl;
      if(slaveParams[index].nRegs > 0)
	{
	
	  slaveParams[index].stRegisters = new mbReadData[slaveParams[index].nRegs];

	  //retrieving tag data
	  i=0;
	  for (pugi::xml_node tag = protocol.child("tag"); tag; tag  = tag.next_sibling("tag"))
	    {
	      //tag NAME
	      //TODO: it should do any check before accept it
	      retrieveCharAttr(&tag,slaveParams[index].stRegisters[i].tagName,"name");
	      //tag ADDRESS
	      retrieveIntAttr(&tag,&slaveParams[index].stRegisters[i].iAddress,"addr");
	      //tag type
	      retrieveCharAttr(&tag,slaveParams[index].stRegisters[i].dataType,"type");   
	      if(!checkTagData(i))
		slaveParams[index].stRegisters[i].isValid = 1;
	      else
		slaveParams[index].stRegisters[i].isValid = 1;
	      //std::cout <<"DEBUG: processed tag: " << slaveParams[index].stRegisters[i].tagName << "," << slaveParams[index].stRegisters[i].iAddress << "," << slaveParams[index].stRegisters[i].dataType << std::endl;
	      i++;
	    }
	}
      else
	slaveParams[index].isValid = 0;
    } 
   
  return 0;
}

/*!function to check slave parameters integrity 
TODO: to improve the check!
*/
int ConfigParser::checkSlaveParams(int i)
{
  int failed = -1;
  if (!checkSlaveName(slaveParams[i].slaveName,i))
    {
      if(!checkSlaveProtocol(slaveParams[i].commType))
	{
	  checkSlaveId(i);
	  failed = 0;		    
	}
    }
  return failed;
}

/*!function to check slave name integrity 
TODO: to improve the check!
*/
int ConfigParser::checkSlaveName(const char * name,int index)
{
  int failed = -1;
  if (index > 0)
    {
      for (int j = index-1  ; j >= 0 ; j--)
	failed = failed && strcmp(name,slaveParams[j].slaveName);
    }
  else
    failed = 0;

  return failed;
}

/*!function to check slave protocol integrity 
TODO: to improve the check!
*/
int ConfigParser::checkSlaveProtocol(const char * protocol)
{
  int failed = -1;
  if(!strcmp(protocol,"MODBUSTCP"))
    failed = 0;

  return failed;
}

/*!function to check slave network id, if none, one is asigned
TODO: to improve the check!
*/
int ConfigParser::checkSlaveId(int index)
{
  int failed = -1;
  int match = 1;
  
  if (slaveParams[index].commId <= 0)
      slaveParams[index].commId = 1;
  
  if(index > 0)
    {
      while(match || slaveParams[index].commId > 1000)
	{
	  match = 0;
	  for (int j = index-1  ; j >= 0 ; j--)
	    {
	      if(slaveParams[index].commId == slaveParams[j].commId)
		{
		  slaveParams[index].commId++;
		  match=1;
		}
	    }

	}
      if(slaveParams[index].commId < 1000)
	{
	  failed=0;
	}
      else
	{
	  slaveParams[index].commId=-1;	  
	  failed = 1;
	}
    }
  else
    {
      failed=0;
    }

  return failed;
}

/*!function to check tag integrity 
TODO: to improve the check!
+ to check name duplicity (Also between slaves!
*/
int ConfigParser::checkTagData(int i)
{
  int failed = -1;

  failed = 0;

  return failed;
}

/*!function for retuning a database parameters instance*/
mbSlaves ConfigParser::retSlaveParams(int slave)
{
  mbSlaves temp;
  
  if(slave < nSlaves && slave >= 0)
    {
      return slaveParams[slave];
    }
  else
    return temp;
}


////private and generic////
/*!(private) number of child nodes in a master node of XML document*/
int ConfigParser::retrieveNumberofNodes(pugi::xml_node* master , const char* concept)
{
  int i=0;

  for (pugi::xml_node node = master->child(concept); node; node = node.next_sibling(concept))
    {
      i++;
    }

  return i;
}

/*!(private) number of child nodes in a XML document*/
int ConfigParser::retrieveNumberofNodes(pugi::xml_document* master , const char* concept)
{
  int i=0;

    for (pugi::xml_node node = master->child(concept); node; node = node.next_sibling(concept))
    {
      i++;
    }

  return i;
 
}

/*!(private) function to take a string attribute from XML parsing*/
int ConfigParser::retrieveCharAttr(pugi::xml_node* db, char*& name, const char* attribute)
{

  int size=0;

  if(name)
    delete(name);
  //better strlen than sizeof, sizeof return always 8
  if( strlen(db->attribute(attribute).value()) >0)
    {
      size =strlen(db->attribute(attribute).value()) + 1;
      name = new char[size];
      strcpy(name,db->attribute(attribute).value());
    }
  else
    name = NULL;

  return 0;  
}
/*!(private) function to take a int attribute from XML parsing
TODO:error check to improve, returning fail if value is not present(i.e)*/
int ConfigParser::retrieveIntAttr(pugi::xml_node* db, int* value, const char* attribute)
{
  //as_int() suppose to return 0  if value is empty
  *value = db->attribute(attribute).as_int();

  return 0;  
}
