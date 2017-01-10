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
  char **sqlQuery = NULL;
  char *initValues = NULL;
  int* nQueries = NULL;
  int ret;

  nQueries = new int;
  //taking db parameters
  parameters = dbParams;
  start();
  if(parameters.isValid)
    {
      //building tables instances
      tables = new DBDataTable*[parameters.numTables];

      for(int i=0;i<parameters.numTables;i++)
	{
	  tables[i] = new DBDataTable(tablesParams[i]);
	  std::cout << "DEBUG: going to SQL creation!" << std::endl;
	  tables[i]->create(&parameters,&nQueries,&sqlQuery);
	  std::cout << "DEBUG: returned from creation! created:" << *nQueries <<"  SQL queries "<< std::endl;
	  //TODO: we should catch exceptions!
	  if (*nQueries > 0)
	    {
	      for(int i=0;i<*nQueries;i++)
		{
		  if(sqlQuery != NULL && sqlQuery[i] != NULL && sqlQuery[i][0] )
		    {
		      std::cout << sqlQuery[i] << std::endl;
		      ret = query(NULL,sqlQuery[i]);
		    }

		}
	    }
	  createTriggersTable();
	}
      //building triggers table
      
    }
  delete sqlQuery;
  delete initValues;
  return 0;
}

/*! function to create the triggers table (if needed) */
int DBInterface::createTriggersTable()
{
  int failed = -1;
  field** triggers;
  int numFields = 0;
  int j = 0;
  for(int i=0;i<parameters.numTables;i++)
    {
      if(tables[i]->isReadTriggered())
	numFields++;
      if(tables[i]->isWriteTriggered())
	numFields++;
    }
  triggers = new field*[numFields];
  for(int i=0;i<parameters.numTables;i++)
    {
      if(tables[i]->isReadTriggered())
	{
	  triggers[j] = new field;
	  tables[i]->retReadTrigger(triggers[j]);
	  triggers[j]->forRTable = i;
	  j++;
	}
      if(tables[i]->isWriteTriggered())
	{
	  triggers[j] = new field;
	  tables[i]->retWriteTrigger(triggers[j]);
	  triggers[j]->forWTable = i;
	  j++;
	}
    }
   

  return failed;
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
      //std::cout << "->ERROR!: PARAMETERS: " << std::endl;
      //std::cout << "->ERROR!: type: "<< parameters.type << " ,hostname:" << parameters.host << " ,database:" << parameters.dbName << " ,user:" << parameters.user << " ,password:" << parameters.pass << std::endl;      
    }
}

//SQL
int DBInterface::storeData()
{
  int ret = 0;
  char *sqlQuery = NULL;
  
  for(int i=0;i < parameters.numTables;i++)
    {
      tables[i]->store(&parameters,&sqlQuery);
      //TODO: we should catch exceptions!
      //std::cout <<"DEBUG: SQL query: " <<  sqlQuery << std::endl;
      ret = ret + query(NULL,sqlQuery);
    }
  return ret;
}

/*!function for returning the number of fields private member 
  of a table*/
int DBInterface::retNumFields(int table)
{
  int fields = 0;
  if(table >=0 && table < parameters.numTables)
    fields = tables[table]->retNumFields();
  return fields;
}
/*!function to return a field tag from table
TODO: it should, for convention, only return once*/
char * DBInterface::retFieldTag(int table, int field)
{ 
  if(table >= 0 && table < parameters.numTables)
    return tables[table]->retFieldTag(field);
  else
    return NULL;
}
/*!function to return a field valid variable 
from table*/
int DBInterface::retFieldValid(int table, int field)
{
  int ret = -1;
  if(table >= 0 && table < parameters.numTables)
    {
      ret = tables[table]->retFieldValid(field);      
    }
  
  return ret;

}
/*!function to return a field value variable
from table*/
int DBInterface::retFieldValue(int table, int field)
{
  int ret = -1;  
  if(table >= 0 && table < parameters.numTables)
    {
      ret = tables[table]->retFieldValue(field);
    }
  
  return ret;
}

/////
/*!function to set a field valid variable from table*/
int DBInterface::setFieldValid(int table, int field, int valid)
{
  int ret = -1;
  if(table >= 0 && table < parameters.numTables)
    {
      ret = tables[table]->setFieldValid(field,valid);      
    }
  
  return ret;

}
/*!function to set a field number value variable
from table*/
int DBInterface::setFieldValue(int table, int field, int value)
{
  int ret = -1;  
  if(table >= 0 && table < parameters.numTables)
    {
      ret = tables[table]->setFieldValue(field,value);
    }
  
  return ret;
}
//linking fields with communications
/*!function to check if a field is already linked*/
int DBInterface::fieldLinked(int table,int field)
{
  static int* link = NULL;
  int linked = 0;
  if(table >= 0 && table < parameters.numTables)
    {
      link = tables[table]->retLink(field);
    
      if(link[0]>= 0 && link[1] >= 0)
	linked = 1;
    }
  return linked;
}
/*!function to return a field linking*/
int* DBInterface::retFieldLink(int table, int field)
{
  static int* link = NULL;
  
  if(table >= 0 && table < parameters.numTables)
    {
      link = tables[table]->retLink(field);
    }

  return link;
}
/*!function to link a field with a slave and tag*/
int DBInterface::fieldLink(int table, int field, int slave, int tag)
{
  int ret = -1;
  if(table >= 0 && table < parameters.numTables)
    {
      tables[table]->setLink(field,slave,tag);
      ret = 0;
    }
  return ret;

}

