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
@File piTriggersTable.cpp
'''Table with all triggers for driving data tables'''
*/

#include "piTriggersTable.h"

///TRIGGERS TABLE FUNCTIONS

/*!default destructor*/
DBTriggersTable::~DBTriggersTable()
{ 
  return;
}
/*!function for creating database tables dinamycally
TODO: only implemented sqlite,MySQL tables!
*/
int DBTriggersTable::create(databaseParameters* dbParameters, char **query, char ***initQuery, int **nTrigs)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::create)" << std::endl;
  int ret=-1;
  int *nTs = NULL;
  char * sql = NULL;
  char ** sqlInit = NULL;

  sql = *query;
  sqlInit = *initQuery;
  nTs = *nTrigs;
 
  if(!strcmp(dbParameters->type,"QSQLITE"))
    {
      creationSqlite(&sql);
      initValuesSqlite(&sqlInit,&nTs);
      ret = 0;
    }
   else if(!strcmp(dbParameters->type,"QMYSQL"))
    {
      creationMysql(&sql);
      initValuesMysql(&sqlInit,&nTs);
      ret = 0;
    }
  
  *query = sql;
  *initQuery = sqlInit;
  *nTrigs = nTs;
  
  return ret;
}
/*!function for creating the database schema, for SQLite databases*/
int DBTriggersTable::creationSqlite(char **sqlQuery)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::creationSqlite)" << std::endl; 
  static char * sql = NULL;
  
  sql = *sqlQuery;
  
  sql = new char[strlen("CREATE TABLE IF NOT EXISTS ") + strlen(parameters.tbName) + strlen(" (ID INTEGER PRIMARY KEY AUTOINCREMENT, TRIGGER TEXT, VALUE INT) ")+5];
  sprintf(sql,"CREATE TABLE IF NOT EXISTS %s (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT, VALUE INT)", parameters.tbName);

  *sqlQuery = sql;

  return 0;
}
/*!function for creating the database schema, for MySQL databases*/
int DBTriggersTable::creationMysql(char **sqlQuery)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::creationMysql)" << std::endl; 
  static char * sql = NULL;
    
  sql = *sqlQuery;
  
  sql = new char[strlen("CREATE TABLE IF NOT EXISTS `") + strlen(parameters.tbName) + strlen("` (ID INTEGER PRIMARY KEY AUTOINCREMENT, TRIGGER TEXT, VALUE INT) ")+5];
  sprintf(sql,"CREATE TABLE IF NOT EXISTS `%s` (ID INTEGER PRIMARY KEY AUTO_INCREMENT, `NAME` TEXT, `VALUE` INT)", parameters.tbName);

  *sqlQuery = sql;

  return 0;

}
/*!function for insert initialization NULL data to a SQLITE NOT LOG type table
*/
int DBTriggersTable::initValuesSqlite(char ***sqlQuery, int **nTrs)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::initValuesSqlite)" << std::endl;
  int ret = -1, i = 0;
  static char ** sql;
  char **noRepeatedFields;
  static int* numNoRepeatedFields= new int(0);
    
  sql = *sqlQuery;

  *numNoRepeatedFields = retNoRepeatedFields(&noRepeatedFields);
  sql = new char*[*numNoRepeatedFields];
  for (i = 0; i < *numNoRepeatedFields;i++)
    {
      sql[i] = new char[strlen("INSERT OR REPLACE INTO `") + strlen(parameters.tbName) + strlen("` (ID,NAME, VALUE) VALUES(COALESCE((SELECT ID from `") + strlen(parameters.tbName) + strlen("` WHERE NAME = '")+ strlen(noRepeatedFields[i]) + strlen("'),100),'") + strlen(noRepeatedFields[i]) + strlen("\',0)")+ 10];
      sprintf(sql[i],"INSERT OR REPLACE INTO `%s` (ID,NAME, VALUE) VALUES(COALESCE((SELECT ID from `%s` WHERE NAME = '%s'),%d),'%s',0)",parameters.tbName,parameters.tbName,noRepeatedFields[i],i+1,noRepeatedFields[i]);  
    }
  ret = 0;

  *sqlQuery = sql;
  *nTrs = numNoRepeatedFields;
    
  return ret;
}
/*!function for insert initialziation NULL data to a MySQL NOT LOG type table
*/
int DBTriggersTable::initValuesMysql(char ***sqlQuery, int **nTrs)
{
  //std::cout << "DEBUG: (inside DBTable::initValuesMysql)" << std::endl;
  int ret = -1, i = 0;
  static char ** sql;
  char **noRepeatedFields;
  static int* numNoRepeatedFields= new int(0);
    
  sql = *sqlQuery;
  
  *numNoRepeatedFields = retNoRepeatedFields(&noRepeatedFields);
  sql = new char*[*numNoRepeatedFields];
  for (i = 0; i < *numNoRepeatedFields;i++)
    {
      sql[i] = new char[strlen("INSERT INTO `") + strlen(parameters.tbName) + strlen("` (ID,NAME, VALUE) VALUES(") + strlen(noRepeatedFields[i]) + strlen(", 0) ON DUPLICATE KEY UPDATE NAME=\"") + strlen(noRepeatedFields[i]) + strlen("\", VALUE=0") + 10];
      sprintf(sql[i],"INSERT INTO `%s` (ID,NAME, VALUE) VALUES(%d,\"%s\", 0) ON DUPLICATE KEY UPDATE NAME=\"%s\", VALUE=0",parameters.tbName,i+1,noRepeatedFields[i],noRepeatedFields[i]);    
    }
 
  ret = 0;
  *sqlQuery = sql;
  *nTrs = numNoRepeatedFields;
  
  return ret;
}

//SQL functions
/*!function for returning a sql command to take every trigger triggered in database table*/
int DBTriggersTable::sqlTrgsTgd(char * & sql)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::sqlTrgsTgd)" << std::endl;
  sql = new char[strlen("SELECT `NAME` from `") + strlen(parameters.tbName) + strlen("` where (`VALUE`= 1)")+5];
  sprintf(sql,"SELECT `NAME` from `%s` where (`VALUE`= 1)", parameters.tbName);

  return 0;
}
//SQL functions
/*!function for returning a sql command to take every trigger triggered in database table*/
int DBTriggersTable::sqlTrgsDone(char * & sql)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::sqlTrgsDone)" << std::endl;
  int failed = -1;
  int strLength = 0;
  int num = 0, j = 0;
  char temp[100];
  sql = NULL;
  for (int i = 0; i < parameters.numFields; i++)
    {
      if (parameters.stField[i].isDone && parameters.stField[i].iValue)
	{
	  strLength += strlen(parameters.stField[i].name) + strlen("\" or name = \"");
	  num++;
	  //std::cout << "DEBUG: (inside DBTriggersTable::sqlTrgsDone) considerando:" << parameters.stField[i].name << "  que está hecho?:" << parameters.stField[i].isDone << "y tiene valor?:" << parameters.stField[i].iValue << std::endl;
	}
    }
  //std::cout << "DEBUG: (inside DBTriggersTable::sqlTrgsDone) strLength:" << strLength << std::endl;
  if (strLength > 0)
    {
      sql = new char[strlen("UPDATE `") + strlen(parameters.tbName) + strlen("` set value = 0 where ( name = \"") + strLength + 5];
      sprintf(sql,"UPDATE `%s` set value = 0 where ( name = \"",parameters.tbName);
      for (int i = 0; i < parameters.numFields; i++)
	{
	  if (parameters.stField[i].isDone  && parameters.stField[i].iValue)
	    {
	      if (j < num)
		{
		  if (j < num-1)
		    {
		      sprintf(temp,"%s\" or name = \"",parameters.stField[i].name);
		      strcat(sql,temp);
		      j++;
		    }
		  else
		    {
		      sprintf(temp,"%s\")",parameters.stField[i].name);
		      strcat(sql,temp);
		      j++;
		    }
		}
	    }
	}
      failed = 0;
    }
  return failed;
}

/*!function for setting on triggers*/
int DBTriggersTable::updtTrgsOn(std::vector <char*> triggersOn)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::updtTrgsOn)" << std::endl;
  int failed = -1;

  for(int j=0; j < parameters.numFields; j++)
    {
      if(parameters.stField[j].isDone)
	parameters.stField[j].iValue = 0;
      for (std::vector< char* >::iterator triggerName = triggersOn.begin() ; triggerName != triggersOn.end(); ++triggerName)
	{
	  if(!strcmp((*triggerName), parameters.stField[j].name))
	    {
	      parameters.stField[j].iValue = 1;
	      parameters.stField[j].isValid = 1;
	      parameters.stField[j].isDone = 0;
	      failed = 0;
	    }
	}
    }
  return 0;
}

/*!function for setting on triggers*/
int DBTriggersTable::updtTrgsDone(std::vector <field*> triggersLst)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::updtTrgsOn)" << std::endl;
  int failed = -1;
  for (std::vector< field* >::iterator trigger = triggersLst.begin() ; trigger != triggersLst.end(); ++trigger)
    {
      if ((*trigger)->isDone != 0)
	{
	  for(int j=0; j < parameters.numFields; j++)
	    {
	      if(!strcmp((*trigger)->name, parameters.stField[j].name))
		{
		  parameters.stField[j].isValid = 1;
		  parameters.stField[j].isDone = 1;
		  failed = 0;
		}
	    }
	}
    }
  return 0;
}

/*!function to return a field tag
TODO: it should, for convention, only return once*/
char * DBTriggersTable::retFieldTag(int field)
{
  if(field >= 0 && field < parameters.numFields)
    return parameters.stField[field].tag;
  else
    return NULL;

}

/*!function to return linking info from tag
*/
int * DBTriggersTable::retLink(int field)
{
  static int link[2];
  link[0] = -1;
  link[1] = -1;
  
  if(field >= 0 && field < parameters.numFields)
    {
      link[0] = parameters.stField[field].fromSlave;
      link[1] = parameters.stField[field].fromTag;
    }
  
 return link;

}
/*!function to return a field valid variable*/
int DBTriggersTable::retFieldValid(int field)
{
  int ret = -1;
  if(field >= 0 && field < parameters.numFields)
    {
      ret = parameters.stField[field].isValid;
    }
  return ret;
}

/*!function to return a field number value variable*/
int DBTriggersTable::retFieldValue(int field)
{
  int ret = -1;  
  if(field >= 0 && field < parameters.numFields)
    {
      ret = parameters.stField[field].iValue;      
    }
  return ret;
}
/*!function for returning not repeated triggers from datatables*/
int DBTriggersTable::retNoRepeatedFields(char ***fieldsNames)
{
  int ret = -1;
  int noRepeated = 0,j=0;
  int equal = 0;
  static   char ** fields;

  fields = *fieldsNames;

  for(int i = 0; i<parameters.numFields;i++)
    {
      if(i==0)
	noRepeated++;
      else
	{
	  for(int k = (i-1) ; k >= 0; k--)
	    {
	      if(!strcmp(parameters.stField[i].name,parameters.stField[k].name))
		  equal++;
	    }
	  if(!equal)
	    noRepeated++;
	    
	  equal = 0;	    
	}      
    }
  fields = new char*[noRepeated];
  for(int i = 0; i<parameters.numFields;i++)
    {
      if(i==0 && j == 0)
	{
	  fields[j] = new char[strlen(parameters.stField[i].name)+5];
	  strcpy(fields[j],parameters.stField[i].name);
	  j++;
	}
      else
	{
	  for(int k = (i-1) ; k >= 0; k--)
	    {
	      if(!strcmp(parameters.stField[i].name,parameters.stField[k].name))
		  equal++;
	    }
	  if(!equal)
	    {
	      if(j<noRepeated)
		{
		  fields[j] = new char[strlen(parameters.stField[i].name)+5];
		  strcpy(fields[j],parameters.stField[i].name); 
		  j++;
		}
	    }
	  equal = 0;	    
	}      
    }
  *fieldsNames = fields;
  return noRepeated;
}

/*!function for returning all triggered fields */
 int DBTriggersTable::retTgsTgd(field*** triggers, int **numberOf)
{
  //std::cout << "DEBUG:(inside DBTriggersTable::retTgsTgd)" << std::endl;
  int ret = -1;
  field ** fields = NULL;
  int *nFields = new int();
  int k = 0;

  *nFields = 0;
  
  for(int i = 0; i < parameters.numFields ; i++)
    {
      if(parameters.stField[i].iValue && !parameters.stField[i].isDone)
	*nFields = *nFields+1;
    }
  if(*nFields >0)
    {
      fields = new field*[*nFields];
      for(int j = 0; j < parameters.numFields ; j++)
	{
	  if(parameters.stField[j].iValue && !parameters.stField[j].isDone)
	    {
	      if(k<*nFields)
		{
		  fields[k] = new field(parameters.stField[j]);
		  k++;
		}
	    }
	}
    }
  *triggers = fields;
  *numberOf = nFields;

  return ret;
}

/*!function for returning all triggers triggered */
int DBTriggersTable::retTgsLst(std::vector <field*> & triggers)
{
  //std::cout << "DEBUG:(inside DBTriggersTable::retTgsLst)" << std::endl;
  int failed = -1;

  int k = 0;

  for (std::vector< field* >::iterator it = triggers.begin() ; it != triggers.end(); ++it)
   {
     delete (*it);
   } 
   triggers.clear();
  
  for(int i = 0; i < parameters.numFields ; i++)
    {
      if(parameters.stField[i].iValue && !parameters.stField[i].isDone)
	{
	  triggers.push_back(new field(parameters.stField[i]));
	  failed = 0;
	}
    }

  return failed;
} 
//
/*!function to set a field valid variable*/
int DBTriggersTable::setFieldValid(int field, int valid)
{
  int ret = -1;
  if(field >= 0 && field < parameters.numFields)
    {
      // std::cout <<"DEBUG: (inside DBTable::setFieldValid function) field:"<<field<<" isValid:" << valid << std::endl;
      parameters.stField[field].isValid = valid;
      ret = 0;
    }
  
  return ret;

}
/*!function to set a field number value variable*/
int DBTriggersTable::setFieldValue(int field, int value)
{
  int ret = -1;  
  if(field >= 0 && field < parameters.numFields)
    {
      if(!strcmp(parameters.stField[field].type,"INT"))
	parameters.stField[field].iValue = value;
      else
	parameters.stField[field].iValue = value;
      ret = 0;
    }
  
  return ret;
}

/*!function to set a linking info to tag
*/
int DBTriggersTable::setLink(int field, int slave, int tag)
{
  int ret = -1;
  if(field >= 0 && field < parameters.numFields)
    {
      parameters.stField[field].fromSlave = slave;
      parameters.stField[field].fromTag = tag;
      ret = 0;
    }
  
 return ret;

}
//
//DEBUG functions
//
/*!function to return a field tag
TODO: it should, for convention, only return once*/
int DBTriggersTable::showTriggers()
{
  for(int i=0;i < parameters.numFields;i++)
    std::cout << parameters.stField[i].tag << std::endl;
  return 0;
}
