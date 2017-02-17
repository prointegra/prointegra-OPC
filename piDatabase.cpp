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
@File piDatabase.cpp
'''Database and tables manager
checks config and create dinamically databases connections and schemas'''
*/

#include "piDatabase.h"

/*! function to take database connection and table parameters for our database interface */
int DBInterface::setup(databaseParameters dbParams, tableParameters* tablesParams)
{
  char **sqlQuery = NULL;
  char *triggersQuery = NULL;
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
	  std::cout << "INFO: creating table nº" << i+1 << " - " << tablesParams[i].tbName <<" instance, and SQL structure..." << std::endl;
	  tables[i] = new DBDataTable(tablesParams[i]);
	  //std::cout << "DEBUG: (inside DBInterface::setup) checking tablesParams.tbTrigger = "<< tablesParams[i].tbTrigger << "  of table = " << i+1 << std::endl;
	  //std::cout << "DEBUG: going to SQL creation!" << std::endl;
	  tables[i]->create(&parameters,nQueries,&sqlQuery);
	  tables[i]->setId(i);
	  //std::cout << "DEBUG: returned from creation! created:" << *nQueries <<"  SQL queries "<< std::endl;
	  //TODO: we should catch exceptions!
	  if (*nQueries > 0)
	    {
	      for(int i=0;i<*nQueries;i++)
		{
		  if(sqlQuery != NULL && sqlQuery[i] != NULL && sqlQuery[i][0] )
		    {
		      //std::cout << sqlQuery[i] << std::endl;
		      ret = query(NULL,sqlQuery[i]);
		    }
		
		}
	    }
	  for(int i=*nQueries-1;i>=0;i--)
	    delete sqlQuery[i];
	}

      createTriggersTable();
      triggersTable->create(&parameters,&triggersQuery,&sqlQuery,&nQueries);
      //TODO: we should catch exceptions!
      //std::cout << "DEBUG: (inside DBInterface::setup) triggers table sql creation: " << triggersQuery << std::endl;
      if(!query(NULL,triggersQuery))
	{
	  for(int i=0;i < *nQueries; i++)
	    {
	      //std::cout << "DEBUG: (inside DBInterface::setup) triggers table sql creationsending query!: " << sqlQuery[i] << std::endl;
	      query(NULL,sqlQuery[i]);
	    }
	}
      ret = 0;
	
  
      for(int i=*nQueries-1;i>=0;i--)
	delete sqlQuery[i];
      delete sqlQuery;
      delete nQueries;
      delete initValues;
    }
  return 0;
}

/*! function to create the triggers table (if needed) */
int DBInterface::createTriggersTable()
{
  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable)" << std::endl;
  int failed = -1;
  field* triggers;
  int numFields = 0;
  int j = 0;
  tableParameters *triggersTableParams = new tableParameters();
  for(int i=0;i<parameters.numTables;i++)
    {
      if(tables[i]->isReadTriggered())
	numFields++;
      if(tables[i]->isWriteTriggered())
	numFields++;
    } 
  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating structure of " << numFields << " fields!" << std::endl;
  triggers = new field[numFields];
  for(int i=0;i<parameters.numTables;i++)
    {
      if(tables[i]->isReadTriggered())
	{
	  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) read trigger found!" << std::endl;
	  //triggers[j] = new field;
	  tables[i]->retReadTrigger(&triggers[j]);
	  triggers[j].forRTable = tables[i]->retId();
	  j++;
	}
      if(tables[i]->isWriteTriggered())
	{
	  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) write trigger found!" << std::endl;
	  //triggers[j] = new field;
	  tables[i]->retWriteTrigger(&triggers[j]);
	  triggers[j].forWTable = tables[i]->retId();
	  j++;
	}
    }
  numFields = j;
  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating table parameters!" << std::endl;
  triggersTableParams->tbName = new char[strlen("triggers")+5];
  sprintf(triggersTableParams->tbName,"triggers");
  triggersTableParams->numFields = numFields;
  triggersTableParams->stField = new field[numFields];
  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating fields!" << std::endl; 
  for (int i = 0; i < numFields ; i++)
    {
      //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy1" << std::endl; 
      triggersTableParams->stField[i].name = new char[strlen(triggers[i].name)+5];
      sprintf(triggersTableParams->stField[i].name, triggers[i].name);
      //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy2" << std::endl;
      triggersTableParams->stField[i].tag = new char[strlen(triggers[i].tag)+5];
      sprintf(triggersTableParams->stField[i].tag, triggers[i].tag);
      //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy3" << std::endl;
      triggersTableParams->stField[i].type = new char[strlen(triggers[i].type)+5];
      sprintf(triggersTableParams->stField[i].type, triggers[i].type);
      triggersTableParams->stField[i].forRTable = triggers[i].forRTable;
      triggersTableParams->stField[i].forWTable = triggers[i].forWTable;      
    }

  //std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating triggers class!" << std::endl; 
  triggersTable = new DBTriggersTable(*triggersTableParams);

  delete triggersTableParams;
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
      if(sqlQuery != NULL)
	delete sqlQuery;
    }
  return ret;
}

/*function for storing data to a  Database's table (by table Id)*/
int DBInterface::storeData(int tableId, std::vector<field> data)
{
  std::cout << "DEBUG: (inside DBInterface::storeData)" << std::endl;
  int failed = -1;
  char *sqlQuery = NULL;
  
  for(int i=0;i < parameters.numTables;i++)
    {
      failed = 0;
      if(tables[i]->retId() == tableId)
	{
	  tables[i]->updateData(data);
	  failed = tables[i]->store(&parameters,&sqlQuery);
	  //TODO: we should catch exceptions!
	  std::cout << "DEBUG: (inside DBInterface::storeData) SQL query:"<<  sqlQuery << std::endl;
	  failed = failed + query(NULL,sqlQuery);
	  if(sqlQuery != NULL)
	    delete sqlQuery;
	}
    }
  return failed;
}

/*function for retrieving data from Database to table memory*/
int DBInterface::retrieveData(int id)
{
  int failed = -1;
  char *sqlQuery;
  char ***table;
  int *x;
  int *y;

  for(int i = 0; i <parameters.numTables;i++)
    {
      if (tables[i]->retId() == id)
	{
	  tables[i]->sqlSelectAll(parameters,sqlQuery);
	  query(NULL,sqlQuery);
	  if(retData(NULL,&table,&x,&y))
	    std::cout <<"ERROR:(inside DBInterface::retDataFrTable) retData return error!" << std::endl;
	  tables[i]->setAllValues(table,*x,*y,1); //1 for skipping id field
	  failed = 0;

	  delTable(table,x,y);
	  if(sqlQuery != NULL)
	    delete sqlQuery;
	}
    }  
  return failed;
}


//
//RETURNING DATA FUNCTIONS
//
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

/*!function for returning data from a table id*/
int DBInterface::retDataFrTable(std::vector <field> & fields, int tableId)
{
  //std::cout << "DEBUG: (inside DBInterface::retDatafrTable)" << std::endl;
  int failed = -1;

  for(int i = 0; i <parameters.numTables;i++)
    {
      if (tables[i]->retId() == tableId)
	{
	  tables[i]->retvFields(fields);
	  failed = 0;
	}
    }  
  return failed;
}

//
//SETTING DATA FUNCTIONS
//
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

/*!function to take tables triggers*/
int DBInterface::takeTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::takeSQLTriggers)" << std::endl; 
  int failed = -1;

  failed = takeTimeTriggers();
  failed = failed & takeSQLTriggers();
  
  return failed;
}

/*!function to take timing triggers*/
int DBInterface::takeTimeTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::takeTimeTriggers)" << std::endl; 
  int failed = -1;
  for(int i = 0; i < parameters.numTables ; i++)
    {
      if(tables[i]->isTimeTriggered())
	{
	  timeTriggersLst.push_back(tables[i]->retId()); 
	}
      if(!tables[i]->isTimeInitialized())
	tables[i]->startTiming();
    }

  return failed;
}

/*!function to add to the internal triggers list the SQL triggers from triggers table*/
int DBInterface::takeSQLTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::takeSQLTriggers)" << std::endl; 
  int failed = -1;
  char *sqlQuery;
  char ***table;
  int *x;
  int *y;
  std::vector <char *> triggersOn;
  //SQL triggers
  triggersTable->sqlTrgsTgd(sqlQuery);
  //std::cout << "DEBUG: (inside DBInterface::retTriggers) sql = " << sql << std::endl;
  query(NULL,sqlQuery);
  if (sqlQuery != NULL)
    delete sqlQuery;
  if(retData(NULL,&table,&x,&y))
    std::cout <<"ERROR:(inside DBInterface::takeTriggers) retData return error!" << std::endl;
  if(*y > 0)
    {
      for(int i =0;i<*y;i++)
	{
	  triggersOn.push_back(new char[strlen(table[i][0]) + 1]);
	  strcpy(triggersOn.at(i),table[i][0]); 
	}     
      //cleaning
      delTable(table,x,y);
      failed = 0;
    }
  triggersTable->updtTrgsOn(triggersOn);
  for (std::vector< char * >::iterator it = triggersOn.begin() ; it != triggersOn.end(); ++it)
    {
      delete (*it);
    }
  triggersOn.clear();
  //get the complete list of triggers, with time triggers also
  triggersTable->retTgsLst(triggersLst);
  return failed;
}

/*!function to reset the triggers done*/
int DBInterface::resetTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::resetTriggers)" << std::endl; 
  int failed = -1;
  
  failed = resetTimeTriggers();
  failed = failed & resetSQLTriggers();
  
  return failed;
}
/*!function to reset the triggers done by time triggers*/
int DBInterface::resetTimeTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::resetTimeTriggers)" << std::endl; 
  int failed = -1;
  for(int i = 0; i < timeTriggersLst.size() ; i++)
    {
      for(int j=0; j < parameters.numTables;j++)
	{
	  if(i>=0)
	    {
	      if (timeTriggersLst.at(i) == tables[j]->retId())
		{
		  tables[j]->startTiming();
		  timeTriggersLst.erase(timeTriggersLst.begin()+i);
		  i--;
		}
	    }
	}
    }
  
  return failed;
}
/*!function to reset the triggers done in the internal triggers list*/
int DBInterface::resetSQLTriggers()
{
  //std::cout << "DEBUG:(inside DBInterface::resetSQLTriggers)" << std::endl; 
  int failed = -1;
  char *sqlQuery;

  failed = triggersTable->updtTrgsDone(triggersLst);
  /* ONLY FOR DEBUG PURPOSES
     showTriggers();
  */
  failed = triggersTable->sqlTrgsDone(sqlQuery);
  if (!failed)
    {
      //std::cout << "DEBUG: (inside DBInterface::resetTriggers) sql = " << sql << std::endl;
      failed = query(NULL,sqlQuery);
    }
  if (sqlQuery != NULL)
    delete sqlQuery;

  return failed;
}
/*!function to mark the writting trigger to a table as done*/
int DBInterface::wTriggerDoneAt(int table)
{
  //std::cout << "DEBUG:(inside DBInterface::wTriggerDoneAt)" << std::endl;
  int failed = -1;
  
  if(triggersLst.size() > 0)
    {
      for(std::vector<field*>::iterator it = triggersLst.begin(); it !=triggersLst.end(); ++it)
	{
	  if((*it)->forWTable == table)
	    {
	      //std::cout << "DEBUG:(inside DBInterface::wTriggerDoneAt) marking:"<< (*it)->name << "  as done!" << std::endl;
	      (*it)->isDone = 1;
	      failed = 0;
	    }
	}
    }

  return failed;
}

/*!function to mark the reading trigger from a table as done*/
int DBInterface::rTriggerDoneAt(int table)
{
  //std::cout << "DEBUG:(inside DBInterface::rTriggerDoneAt)" << std::endl;
  int failed = -1;
  
  if(triggersLst.size() > 0)
    {
      for(std::vector<field*>::iterator it = triggersLst.begin(); it !=triggersLst.end(); ++it)
	{
	  if((*it)->forRTable == table)
	    {
	      //std::cout << "DEBUG:(inside DBInterface::wTriggerDoneAt) marking:"<< (*it)->name << "  as done!" << std::endl;
	      (*it)->isDone = 1;
	      failed = 0;
	    }
	}
    }

  return failed;
}
/*!function for returning a list of tables Write triggered*/
int DBInterface::retWTabsList(std::vector <int> & tablesList)
{
  //std::cout << "DEBUG: (inside DBInterface::retWTabsList)" << std::endl;
  int failed = -1;
  int vectorSize = 0;
  
  tablesList.clear();
  if(triggersLst.size() > 0)
    {
      for(std::vector<field*>::iterator it = triggersLst.begin(); it !=triggersLst.end(); ++it)
	{
	  if((*it)->forWTable >= 0)
	    {
	      tablesList.push_back((*it)->forWTable);
	      failed = 0;
	    }
	}
    }

  return failed;
}

/*!function for returning a list of tables Read triggered*/
int DBInterface::retRTabsList(std::vector <int> & tablesList)
{
  //std::cout << "DEBUG: (inside DBInterface::retRTabsList)" << std::endl;
  int failed = -1;
  int vectorSize = 0;
  
  tablesList.clear();
  if(triggersLst.size() > 0)
    {
      for(std::vector<field*>::iterator it = triggersLst.begin(); it !=triggersLst.end(); ++it)
	{
	  if((*it)->forRTable >= 0)
	    {
	      tablesList.push_back((*it)->forRTable);
	      failed = 0;
	    }
	}
    }
   if(timeTriggersLst.size() > 0)
    {
      for(std::vector<int>::iterator it = timeTriggersLst.begin(); it !=timeTriggersLst.end(); ++it)
	{	
	  tablesList.push_back(*it);
	  failed = 0;  
	}
    } 

  return failed;
}

//
//linking fields with communications functions
//
/*!function to return a field linking*/
std::vector<std::vector <int>> DBInterface::retFieldLink(int table, int field)
{
  std::vector<std::vector <int>> link;
  if(table >= 0 && table < parameters.numTables)
    {
      link = tables[table]->retLink(field);
    }

  return link;
}
/*!function to link a field with a slave and tag*/
int DBInterface::setFieldLink(int table, int field, int slave, int tag)
{
  int ret = -1;
  if(table >= 0 && table < parameters.numTables)
    {
      tables[table]->setLink(field,slave,tag);
      ret = 0;
    }
  return ret;
}

/*!function to link a field with a slave and tag*/
int DBInterface::fieldLink(int table, int field, int slave, int tag)
{
  //std::cout << "DEBUG: (inside DBInterface::fieldLink)" << std::endl;
  int ret = -1;
  if(table >= 0 && table < parameters.numTables)
    {
      tables[table]->setLink(field,slave,tag);
      ret = 0;
    }
  return ret;
}

//
//deleting elements functions
/*! function for deleting a table returned from a database query function*/
int DBInterface::delTable(char ***& table, int *&x, int *&y)
{
  int rows = 0;
  //std::cout << "DEBUG: (inside DBInterface::delTable)" << std::endl;
   if(y != NULL)
    {
      delete y;
    }
  if(x != NULL)
    {
      rows = *x;
      delete x;
    }
  for (int i = (rows-1); i >= 0; i--)
    delete [] table[i];
  if (table != NULL)
    delete table;
  
  return 0;
}

//
//DEBUG FUNCTIONS
/*!function to show triggers list members*/
int DBInterface::showTriggers()
{
  std::cout << "DEBUG: (inside DBInterface::showTriggers)" << std::endl;
  int failed = -1;
  for (std::vector<field*>::iterator it = triggersLst.begin(); it != triggersLst.end(); ++it)
    {
      std::cout << "<******>" << std::endl;
      std::cout << "<- Name:" << (*it)->name << std::endl;
      std::cout << "<- isValid:" << (*it)->isValid << std::endl;
      std::cout << "<- value:" << (*it)->iValue << std::endl;
      std::cout << "<- isDone:"<< (*it)->isDone << std::endl;
    }
  return failed;

}
