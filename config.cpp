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
/**
@file config.c
*/

#include "config.h"
//constants
const char* sVERSION = "v0.0.1 ALPHA";

/*! Constructor*/
configParser::configParser(char* path)
{
  //initialization of file
  pugi::xml_parse_result result = doc.load_file(path); 
  cout << "Load result: " << result.description()  << endl;
  return;   
}
/*! Destructor*/
configParser::~configParser()
{
  if (databaseParams != NULL)
    delete databaseParams;
       
  return;   
}
//database functions
/*!retrieving database parameters*/
int configParser::retrieveDBParams()
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
      retrieveCharAttr(&db,&databaseParams[i].internalName,"name");
      //retrieve database qt driver
      retrieveCharAttr(&db,&databaseParams[i].type,"qtdriver");
      //retrieve database hostname
      retrieveCharAttr(&db,&databaseParams[i].host,"hostname");
      //retrieve database id name
      retrieveCharAttr(&db,&databaseParams[i].dbName,"dbName");    
      //retrieve database user
      retrieveCharAttr(&db,&databaseParams[i].user,"user");
      //retrieve database id name user password
      retrieveCharAttr(&db,&databaseParams[i].pass,"password");
      //number of tables
      databaseParams[i].numTables = retrieveNumberofNodes(&db,"table");
      //if everything's allright
      if(!checkDBParams(i))
	{
	  std::cout << "DEBUG: processing tables from "<< databaseParams[i].internalName <<" database..." << std::endl;
	  retrieveTableAttrs(&db,i,databaseParams[i].numTables);
	  i++;
	}
    }
  //i = number of databases created 
  return 0;
}

int configParser::checkDBParams(int i)
{
  if (databaseParams[i].internalName != NULL)
    {
      if(!checkDBType(databaseParams[i].type))
	{
	  //if(!checkDBHostname(type,hostname, user, password))
	  //  {
	  //    if(!checkDBName(type,dbName)
	  //	{

	  return 0;
		    
		  //	}
		//}
	}
    }
  return -1;
}

int configParser::retrieveCharAttr(pugi::xml_node* db, char** name, const char* attribute)
{
  char *newName;
  int size=0;

  newName = *name;
  delete(newName);
  //better strlen than sizeof, sizeof return always 8
  if( strlen(db->attribute(attribute).value()) >0)
    {
      size =strlen(db->attribute(attribute).value()) + 1;
      newName = new char[size];
      strcpy(newName,db->attribute(attribute).value());
    }
  else
    newName = NULL;

  *name = newName;
  return 0;  
}

int configParser::retrieveIntAttr(pugi::xml_node* db, int* value, const char* attribute)
{

  int size=0;

  //as_int() suppose to return 0  if value is empty
  *value = db->attribute(attribute).as_int();

  return 0;  
}

int configParser::checkDBType(const char* type)
{
  char * temp;

  temp = new char[strlen(type)+1];
  strcpy(temp,type);
  for(int i=0;i<strlen(temp);i++)
    temp[i] = toupper(temp[i]);
  if(!strcmp(temp,"QSQLITE") || !strcmp(temp,"QTDS"))
    {
      delete temp;
      return 0;
    }
  cout << "ERROR: DATABASE " << temp << " DOESN'T EXIST OR IS NOT IMPLEMENTED" << endl;
  return -1;
}


databaseParameters configParser::retDBParams(int database)
{

  char *type = NULL;
  char *dbName = NULL;
  char *name = NULL;
  int saveMode;
  int numTables;
  char *tableName = NULL;
  char *user = NULL;
  char * password = NULL;
  char *hostname = NULL;

  databaseParameters temp;
  
  if(database < nDBs && database >= 0)
    {
      return databaseParams[database];
    }
  else
    return temp;
}
/*!(private) number of child nodes in a master node of XML document*/
int configParser::retrieveNumberofNodes(pugi::xml_node* master , const char* concept)
{
  int i=0;

  for (pugi::xml_node node = master->child(concept); node; node = node.next_sibling(concept))
    {
      i++;
    }

  return i;
 
}
/*!(private) number of child nodes in a XML document*/
int configParser::retrieveNumberofNodes(pugi::xml_document* master , const char* concept)
{
  int i=0;

    for (pugi::xml_node node = master->child(concept); node; node = node.next_sibling(concept))
    {
      i++;
    }

  return i;
 
}

/*! function to retrieve all data from table in database, and creating it in memory struct */
int configParser::retrieveTableAttrs(pugi::xml_node* db, int dbNumber, int numTables)
{

  int numFields;
  char* fieldName = NULL;
  char* fieldTagName = NULL;
  char* fieldType= NULL;
  int i;

  //defining number of tables in db
  i = numTables;
  std::cout << "number of tables = " << numTables << std::endl;
  tablesParams[dbNumber] = new tableParameters[i];
  //retrieving table parameters
  i=0;
  for (pugi::xml_node table = db->child("table"); table; table = table.next_sibling("table"))
    {
      retrieveCharAttr(&table,&tablesParams[dbNumber][i].tbName,"name");
      retrieveCharAttr(&table,&tablesParams[dbNumber][i].tbTrigger,"tagTrigger");
      retrieveIntAttr(&table,&tablesParams[dbNumber][i].tbTriggerTime,"timeTrigger");
      //tags
      numFields = retrieveNumberofNodes(&table,"tag");
      std::cout << "number of tablesfields in table = " << i << ", igual a " << numFields << std::endl;
      tablesParams[dbNumber][i].field = new char**[numFields];
      for(int j= 0; j < numFields ; j++)
	tablesParams[dbNumber][i].field[j] = new char*[3];
      int k=0;
      for (pugi::xml_node tag = table.child("tag"); tag; tag = tag.next_sibling("tag"))
	{
	  std::cout << "retrieving data for tag = " << k+1 << std::endl;
	  retrieveCharAttr(&tag,&fieldName,"name");
       	  retrieveCharAttr(&tag,&fieldTagName,"tagName");
	  retrieveCharAttr(&tag,&fieldType,"type");
	  std::cout << "copying data for tag = " << k+1 << std::endl;
	  tablesParams[dbNumber][i].field[k][0] = new char[sizeof(fieldName)+1];
	  strcpy(tablesParams[dbNumber][i].field[k][0],fieldName);
	  std::cout << "name in database = " <<tablesParams[dbNumber][i].field[k][0] << std::endl;
	  tablesParams[dbNumber][i].field[k][1] = new char[sizeof(fieldTagName)+1];
	  strcpy(tablesParams[dbNumber][i].field[k][1],fieldTagName);
	  std::cout << "name in communications = " <<tablesParams[dbNumber][i].field[k][1] << std::endl;
	  tablesParams[dbNumber][i].field[k][2] = new char[sizeof(fieldType)+1];
	  strcpy(tablesParams[dbNumber][i].field[k][2],fieldType);
	  std::cout << "type = " <<tablesParams[dbNumber][i].field[k][2] << std::endl;
	  
	  k++;
	}

      i++;
    } 
 
   
  return 0;
}
