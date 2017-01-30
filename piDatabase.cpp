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
		      std::cout << sqlQuery[i] << std::endl;
		      ret = query(NULL,sqlQuery[i]);
		    }
		}
	    }
	}

      //building triggers table
      for(int i=*nQueries-1;i>=0;i--)
	delete sqlQuery[i];
      createTriggersTable();
      triggersTable->create(&parameters,&triggersQuery,&sqlQuery,&nQueries);
      //TODO: we should catch exceptions!
      std::cout << "DEBUG: (inside DBInterface::setup) triggers table sql creation: " << triggersQuery << std::endl;
      if(!query(NULL,triggersQuery))
	{
	  for(int i=0;i < *nQueries; i++)
	    {
	            std::cout << "DEBUG: (inside DBInterface::setup) triggers table sql creationsending query!: " << sqlQuery[i] << std::endl;
	      query(NULL,sqlQuery[i]);
	    }
	}
      ret = 0;
    }
  
  for(int i=*nQueries-1;i>=0;i--)
    delete sqlQuery[i];
  delete sqlQuery;
  delete nQueries;
  delete initValues;
  
  return 0;
}

/*! function to create the triggers table (if needed) */
int DBInterface::createTriggersTable()
{
  std::cout << "DEBUG:(inside DBInterface::createTriggersTable)" << std::endl;
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
	  std::cout << "DEBUG:(inside DBInterface::createTriggersTable) write trigger found!" << std::endl;
	  //triggers[j] = new field;
	  tables[i]->retWriteTrigger(&triggers[j]);
	  triggers[j].forWTable = tables[i]->retId();
	  j++;
	}
    }
  numFields = j;
  std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating table parameters!" << std::endl;
  triggersTableParams->tbName = new char[strlen("triggers")+5];
  sprintf(triggersTableParams->tbName,"triggers");
  triggersTableParams->numFields = numFields;
  triggersTableParams->stField = new field[numFields];
   std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating fields!" << std::endl; 
  for (int i = 0; i < numFields ; i++)
    {
      std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy1" << std::endl; 
      triggersTableParams->stField[i].name = new char[strlen(triggers[i].name)+5];
      sprintf(triggersTableParams->stField[i].name, triggers[i].name);
      std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy2" << std::endl;
      triggersTableParams->stField[i].tag = new char[strlen(triggers[i].tag)+5];
      sprintf(triggersTableParams->stField[i].tag, triggers[i].tag);
      std::cout << "DEBUG:(inside DBInterface::createTriggersTable) memcpy3" << std::endl;
      triggersTableParams->stField[i].type = new char[strlen(triggers[i].type)+5];
      sprintf(triggersTableParams->stField[i].type, triggers[i].type);
      triggersTableParams->stField[i].forRTable = triggers[i].forRTable;
      triggersTableParams->stField[i].forWTable = triggers[i].forWTable;      
    }

  std::cout << "DEBUG:(inside DBInterface::createTriggersTable) creating triggers class!" << std::endl; 
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
    }
  return ret;
}

/*function for retrieving data from Database to table memory*/
int DBInterface::retrieveData(int id)
{
  int failed = -1;
  char *sql;
  char ***table;
  int *x;
  int *y;

  for(int i = 0; i <parameters.numTables;i++)
    {
      if (tables[i]->retId() == id)
	{
	  tables[i]->sqlSelectAll(parameters,sql);
	  query(NULL,sql);
	  if(retData(NULL,&table,&x,&y))
	    std::cout <<"ERROR:(inside DBInterface::retDataFrTable) retData return error!" << std::endl;
	  tables[i]->setAllValues(table,*x,*y,1); //1 for skipping id field
	  failed = 0;
	  
	  delete sql;
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
/*!function for returning triggers triggered to write/read data to/from tables*/
int DBInterface::retTriggers(field** & triggers, int* & nTriggers)
{
  //std::cout << "DEBUG:(inside DBInterface::retTriggers)" << std::endl;
  int failed = -1;
  char *sql;
  char ***table;
  char **triggersOn;
  int *x;
  int *y;
 
  triggersTable->sqlTgsTgd(sql);
  //std::cout << "DEBUG: (inside DBInterface::retTriggers) sql = " << sql << std::endl;
  query(NULL,sql);
  if(retData(NULL,&table,&x,&y))
    std::cout <<"ERROR:(inside DBInterface::retTriggers) retData return error!" << std::endl;
  if(*y > 0)
    {
      triggersOn = new char*[*y];
  
      for (int i=0;i<*y;i++)
	{
	  triggersOn[i] = new char[strlen(table[i][0])+5];
	  strcpy(triggersOn[i],table[i][0]);
	}
    
      triggersTable->updateTriggersOn(triggersOn,*y);
      //std::cout << "DEBUG:(inside DBInterface::retTriggers) calling triggersTable->retTgsTgd"<< std::endl;  
      triggersTable->retTgsTgd(&triggers,&nTriggers);
  
      //std::cout << "DEBUG:(inside DBInterface::retTriggers) number of triggers:" << *nTriggers << std::endl;
    }
  else
    {
      nTriggers = new int();
      *nTriggers = 0;
    }
  //std::cout << "DEBUG:(inside DBInterface::retTriggers) deleting memory"<< std::endl;
  if(*y >0)
    {
      //cleaning
      for(int j =*y-1;j>=0;j--)
	{
	  for(int k = *x-1;k>=0;k--)
	    delete (table[j][k]);
	  delete table[j];
	}
      delete table;
      for(int l = *y-1;l>=0;l--)
	delete triggersOn[l];
      delete triggersOn;
    }
  delete x;
  delete y;
  delete sql;
  // 
  return 0;
}

/*!function for returning data to write from table, because is triggered*/
int DBInterface::retDataToWrite(field** stTriggers, int *nTriggers, field ****tagsToWrite, int **nTables, int ***nFields)
{
  //std::cout <<"DEBUG:(inside DBInterface::retDataToWrite)" << std::endl;
  int failed = -1;
  int **lTables;
  int *num = NULL;
  field ***dataFields = NULL;
  int **numFields = NULL;
  char *sql = new char[100];
  char ***table;
  int *x;
  int *y;

  //dataFields = *tagsToWrite;
  //numFields = *nFields;
  //num = *nTables;

  //std::cout <<"DEBUG:(inside DBInterface::retDataToWrite) number of triggers in database:" << *nTriggers << std::endl;
  if(!retTablesWList(stTriggers,nTriggers,&lTables,&num))
    {
      dataFields = new field**[*num];
      numFields = new int*[*num];
     
      for(int i = 0; i < *num; i++)
      {
	delete sql;
	tables[*lTables[i]]->sqlSelectAll(parameters,sql);
	query(NULL,sql);
	if(retData(NULL,&table,&x,&y))
	  std::cout <<"ERROR:(inside DBInterface::retDataToWrite) retData return error!" << std::endl;
	tables[*lTables[i]]->setAllValues(table,*x,*y,1); //1 for skipping id field
        tables[*lTables[i]]->retFields(&dataFields[i],&numFields[i]);
      }
      delete sql;

      //freeing lTables
      for(int j = *num-1; j >= 0; j--)
      {
	  delete[] lTables[j];
      }     
      delete lTables;
      failed = 0;
    }
  else
      std::cout <<"ERROR:(inside DBInterface::retDataToWrite) pointers where not initialized!!"<< std::endl;
  
  *nTables = num;
  *tagsToWrite = dataFields;
  *nFields = numFields;
  return failed;  
}

/*!function for returning a list of tables Write triggered*/
int DBInterface::retTablesWList(field** stTriggers, int *nTriggers,int ***lTables, int ** nTables)
{
  //std::cout <<"DEBUG:(inside DBInterface::retTablesWList)" << std::endl;
  int failed = -1, different = 0;
  static int **tables;
  int **temp;
  static int *num;

  //num = *nTables;
  //tables = *lTables;
  
  num = new int(0);
  tables = new int*[1];
  *num = 0;

  for(int i=0; i<*nTriggers;i++)
    {
      if(stTriggers[i]->forWTable>=0)
	{
	  if(*num ==0)
	    {
	      *num = *num+1;
	      tables[0] = new int(stTriggers[i]->forWTable);
	      failed = 0;
	    }
	  else
	    {
	      different = 1;
	      for(int j=0;j<*num;j++)
		{
		  if(*tables[j] == stTriggers[i]->forWTable)
		    different = 0;
		}
	      if(different)
		{
		  temp = new int*[*num];
		  for(int j=0;j<*num;j++)
		    temp[j] = new int(*tables[j]);
	        
		  for(int j = *num-1; j>=0;j--)
		    delete tables[j];	  
		  delete[] tables;
		  
		  *num = *num+1;		  
		  tables = new int*[*num];
		  for(int j=0;j<(*num-1);j++)
		    tables[j] = new int(*temp[j]);
		  tables[*num-1] = new int(stTriggers[i]->forWTable);
		  //deleting temp
		  for(int j = *num-2; j>=0;j--)
		    delete temp[j];
		  delete[] temp;
		}    
	    }
	}
    }

  *nTables = num;
  *lTables = tables;
  return 0;
}

/*!function for returning a list of tables Write triggered*/
int DBInterface::retWTabsList(field** stTriggers, int *nTriggers,std::vector <int> & tablesList)
{
  //std::cout << "DEBUG: (inside DBInterface::retWTabsList)" << std::endl;
  int failed = -1;
  int vectorSize = 0;
  
  tablesList.clear();
  if(*nTriggers > 0)
    {
      for(int i=0; i < *nTriggers ; i++)
	{
	  if(stTriggers[i]->forWTable >= 0)
	    {
	      tablesList.push_back(stTriggers[i]->forWTable);
	      failed = 0;
	    }
	}
    }

  return failed;
}

/*!function for returning data from a table id*/
int DBInterface::retDataFrTable(std::vector <field> & fields, int tableId)
{
  int failed = -1;

  for(int i = 0; i <parameters.numTables;i++)
    {
      if (tables[i]->retId() == tableId)
	{
	  tables[i]->retvFields(fields);
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
/*!function to reset triggers*/
int DBInterface::wTriggerDone(int table)
{
  int failed = -1;

  /*
  for(int i = 0; i < numTriggers;i++)
    {
      if(stTriggers[i]->forWTable >= 0)
	if(tabled == numTable) //it's the table to reset it's trigger
	  {
	    //triggersTable->sqlResetTg(sql,stTriggers[i]->name);
	    query(NULL,sql);
	    failed = 0;
	  }
	else //it isn't 
	  tabled++;
    }
  */
  return failed;
}

//
//linking fields with communications functions
//
/*!function to check if a field is already linked*/
int DBInterface::fieldLinked(int table,int field)
{
  /*
  static int* link = NULL;
  int linked = 0;
  if(table >= 0 && table < parameters.numTables)
    {
      link = tables[table]->retLink(field);
    
      if(link[0]>= 0 && link[1] >= 0)
	linked = 1;
    }
  */
  return 0;
}
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
