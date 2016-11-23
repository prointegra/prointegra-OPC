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

/*!retrieving database parameters*/
int configParser::retrieveDBParams()
{
  int i=0;
  int nTables = 0;
  int size;
  char *type = NULL;
  char *dbName = NULL;
  char *name = NULL;
  int saveMode;
  int numTables;
  char *tableName = NULL;
  char *user = NULL;
  char * password = NULL;
  char *hostname = NULL;
  
  //1 number of databases
  for (pugi::xml_node db = doc.child("db"); db; db = db.next_sibling("db"))
    {
      i++;
    }
  //number of DB instances defined
  nDBs = i;
  ////
  databaseParams = new databaseParameters[nDBs];
  tablesParams = new tableParameters*[nDBs];
  i = 0;
  //capturing data from databases;
  for (pugi::xml_node db = doc.child("db"); db; db = db.next_sibling("db"))
    {
      //retrieve database name (name of whole entity, database name + hostname +user + password + qtdriver + number of tables...)
      retrieveCharAttr(&db,&name,"name");
      //retrieve database qt driver
      retrieveCharAttr(&db,&type,"qtdriver");
      //retrieve database hostname
      retrieveCharAttr(&db,&hostname,"hostname");
      //retrieve database id name
      retrieveCharAttr(&db,&dbName,"dbName");    
      //retrieve database user
      retrieveCharAttr(&db,&user,"user");
      //retrieve database id name user password
      retrieveCharAttr(&db,&password,"password");
      //retrieve database id name user password
      retrieveIntAttr(&db,&nTables,"tables");
      //if everything's allright
      if(!checkDBParams(name,type,hostname,dbName,user,password,nTables))
	{
	  databaseParams[i] = {  type,hostname,dbName,name,user,password,nTables };
	  tablesParams[i]= new tableParameters;
	  retrieveTableAttrs(&db,&tablesParams[i],nTables);
	  i++;
	}
    }
  //i = number of databases created 
  return 0;
}

int configParser::checkDBParams(const char* name, const char* type, const char* hostname, const char* dbName, const char* user, const char* password, int nTables)
{
  if (name != NULL)
    {
      if(!checkDBType(type))
	{
	  //if(!checkDBHostname(type,hostname, user, password))
	  //  {
	  //    if(!checkDBName(type,dbName)
	  //	{
		  if(nTables>0)
		    {
		      return 0;
		    }
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

  databaseParameters temp = { type, hostname, dbName, name, user, password, 0 };
  
  if(database < nDBs && database >= 0)
    {
      return databaseParams[database];
    }
  else
    return temp;
}
/*! fucntion to retrieve all data from tables in db, and creating it in memory struct */
int configParser::retrieveTableAttrs(pugi::xml_node* db, tableParameters** rtables,int nTables)
{
  tableParameters* tables;
  char * dbInternalName;
  char * tbName;
  char * tbTrigger;
  int tbTriggerTime;
  int numFields;
  char* field[3];

  //pointer copy
  //tables = *rtables;
  int i=0;
  for (pugi::xml_node table = db->child("table"); table; table = table.next_sibling("table"))
    {
      i++;
    }
  //tables = new tableParams[nTables];
  std::cout << "TABLES DEFINED : " << i << std::endl;
  
  return 0;
}
