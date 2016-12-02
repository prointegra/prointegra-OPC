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
@File piDatabase.cpp
'''Database and tables manager
checks config and create dinamically databases connections and schemas'''
*/

#include "piDatabase.h"

/*! function to take database connection and table parameters for our database interface */
int DBInterface::setup(databaseParameters dbParams, tableParameters* tablesParams)
{
  char *sqlQuery = NULL;
  int ret;
  
  //taking db parameters
  parameters = dbParams;
  start();
  if(parameters.isValid)
    {
      //building tables instances
      tables = new DBTable*[parameters.numTables];

      for(int i=0;i<parameters.numTables;i++)
	{
	  tables[i] = new DBTable(tablesParams[i]);
	  tables[i]->create(&parameters,&sqlQuery);
          //TODO: we should catch exceptions!
	  std::cout << sqlQuery << std::endl;
	  ret = query(NULL,sqlQuery);


	}
    }
  delete sqlQuery;
  return 0;
}


/*! function to open connection to database */
int DBInterface::start()
{
  //SQLSERVER EXAMPLE
  //if(database->open("QTDS7","PROINTEG-3FCFDD","master","sa","prointegra"))
  //SQLITE 
  if(qtDatabase::open(parameters.type,parameters.host,parameters.dbName,parameters.user,parameters.pass))
    {
      std::cout << "ERROR!: CAN'T OPEN DATABASE " << parameters.internalName << std::endl;
    }
}


//check tables and create them if not exist
//personalize!
int DBInterface::checkAndCreate()
{
  //table "recepcion"
  //index,date,time,silo de recepcion INT, tipo de producto STRING, kilos INT, matricula del camion STRING,proveedor STRING, vacio1 STRING, vacio2 STRING, vacio3 STRING, vacio4 STRING, vacio5 STRING;
  //
  //table "proceso"
  //index, date, time, silo recepcion INT, silo limpia INT, tipo de producto STRING
  //
  //table "salida"
  //index, date, time, juliano envasado INT, juliano limpia INT, silo limpia INT, tipo de producto STRING

  int ret;
  char       buf[80];
 
  std::cout << "DEBUG: Creating table \"recepcion\"" << std::endl;
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS recepcion(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,SILO_RECEPCION INT,TIPO_DE_PRODUCTO INT,KILOS INT, MATRICULA TEXT,PROVEEDOR INT,VACIO1 INT,VACIO2 INT,VACIO3 INT,VACIO4 INT,VACIO5 INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE recepcion" << std::endl;
    }
  std::cout << "DEBUG: Creating table \"proceso\"" << std::endl;
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS proceso(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,SILO_RECEPCION INT,SILO_LIMPIA INT, TIPO_DE_PRODUCTO INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE proceso" << std::endl;
    }
  //
    std::cout << "DEBUG: Creating table \"julianos\"" << std::endl;
    ret = query(NULL,"CREATE TABLE IF NOT EXISTS julianos(SILO INT NOT NULL UNIQUE,JULIANO INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
   ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(5,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(6,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(7,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(8,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    } 
  std::cout << "DEBUG: Creating table \"producto\"" << std::endl;
  
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS producto(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,JULIANO_ENVASADO INT,JULIANO_LIMPIA INT, TIPO_DE_PRODUCTO INT, SILO_LIMPIA INT,KILOS INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE producto" << std::endl;
    }

  return 0;
}



///TABLE FUNCTIONS
/*! function to take table parameters for our table in database interface*/
DBTable::DBTable(tableParameters tableParams)
{
  parameters = tableParams;
  return;
}
DBTable::~DBTable()
{

  return;
}
/*!function for creating the tale dinamycally
TODO: only implemented sqlite tables!
*/
int DBTable::create(databaseParameters* parameters, char **query)
{
  char *sqlQuery = NULL;
  int ret;

  sqlQuery = *query;
  
  if(!strcmp(parameters->type,"QSQLITE"))
    {
      creationSqlite(&sqlQuery);
      ret = 0;
    }
  else if(!strcmp(parameters->type,"QTDS"))
    {
      sqlQuery=NULL;
      ret = -1;
    }
  else
    {
      sqlQuery=NULL;
      ret = -1;
    }
  *query = sqlQuery;
  
  return ret;
}

int DBTable::creationSqlite(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("CREATE TABLE IF NOT EXISTS ") + strlen(parameters.tbName) + strlen(" (ID INTEGER PRIMARY KEY AUTOINCREMENT,")+5];
  strcpy(temp,"CREATE TABLE IF NOT EXISTS ");
  strcat(temp,parameters.tbName);
  strcat(temp," (ID INTEGER PRIMARY KEY AUTOINCREMENT,");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);
  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      temp = new char[strlen(sqlQuery)+5];
      strcpy(temp,sqlQuery);
      delete sqlQuery;
	
      field = new char[strlen(parameters.stField[i].type) + strlen(parameters.stField[i].name) + 5];
      strcpy(field,parameters.stField[i].name);
      //different types
      if(!strcmp(parameters.stField[i].type,"DATE"))
	strcat(field," DATE");
      if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	strcat(field," INT");
      if(!strcmp(parameters.stField[i].type,"TIME"))
	strcat(field," TIME");
      if(!strcmp(parameters.stField[i].type,"STRING"))
	strcat(field," TEXT");      
      if(i < (parameters.numFields-1))
	strcat(field,",");
      /////
      sqlQuery = new char[strlen(temp)+strlen(field)+5];
      strcpy(sqlQuery,temp);
      strcat(sqlQuery,field);
      delete field;
    }
  strcat(sqlQuery,")");
  delete temp;
  *sql = sqlQuery;
  return 0;


}
