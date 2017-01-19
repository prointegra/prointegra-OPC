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
  std::cout << "DEBUG: (inside DBTriggersTable::create)" << std::endl;
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
  std::cout << "DEBUG: (inside DBTriggersTable::creationSqlite)" << std::endl; 
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
  std::cout << "DEBUG: (inside DBTriggersTable::creationMysql)" << std::endl; 
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
  std::cout << "DEBUG: (inside DBTriggersTable::initValuesSqlite)" << std::endl;
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
  std::cout << "DEBUG: (inside DBTable::initValuesMysql)" << std::endl;
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

/*!function for store data to the table
TODO: only implemented sqlite,MySQL tables!
*/
/*
int DBTriggersTable::store(databaseParameters* parameters,char **query)
{
  char *sqlQuery = NULL;
  int ret;

  sqlQuery = *query;
  
  if(!strcmp(parameters->type,"QSQLITE"))
    {
      storeSqlite(&sqlQuery);
      ret = 0;
    }
  else if(!strcmp(parameters->type,"QMYSQL"))
    {
      storeMysql(&sqlQuery);
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
*/
/*!function for storing data to a SQLITE table
*/
/*
int DBTriggersTable::storeSqlite(char **sql)
{
  char *sqlQuery = NULL;
  int ret = -1;

  sqlQuery = *sql;
  if(!strcmp(parameters.tbType,"LASTVALUE"))
    ret = updateSqlite(&sqlQuery);
  else
    ret = insertSqlite(&sqlQuery);
  
  *sql = sqlQuery;
}
*/
/*!function for inserting data to a sqlite table
*/
/*
int DBTriggersTable::insertSqlite(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;

  char *values;
  char *tmpValues;
  int first = 1;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("INSERT INTO ") + strlen(parameters.tbName) + strlen(" (")+7];
  strcpy(temp,"INSERT INTO ");
  strcat(temp,parameters.tbName);
  strcat(temp,"(");

  tmpValues = new char[strlen("VALUES (") +5];
  strcpy(tmpValues,"VALUES (");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);
  values =  new char[strlen(tmpValues)+5];
  strcpy(values,tmpValues);
  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      delete tmpValues;
      temp = new char[strlen(sqlQuery)+5];
      tmpValues = new char[strlen(values)+5];
      strcpy(temp,sqlQuery);
      strcpy(tmpValues,values);
      delete values;
      delete sqlQuery;
      
      if(parameters.stField[i].isValid)
	{
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      field = new char[parameters.stField[i].iValue +5];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+5];
	  values = new char[strlen(tmpValues)+strlen(field)+5];
	  strcpy(sqlQuery,temp);
	  strcpy(values,tmpValues);
	  if(!first)
	    {
	      strcat(values,",");
	      strcat(sqlQuery,",");
	    }
	  else
	    first = 0;
	  strcat(values,field);
	  strcat(sqlQuery,parameters.stField[i].name);
	  delete field;
	}
      else
	{
	  sqlQuery = new char[strlen(temp)+5];
	  values = new char[strlen(tmpValues)+5];
	  strcpy(sqlQuery,temp);
	  strcpy(values,tmpValues);
	}
    }
  strcat(sqlQuery,") ");
  strcat(values,") ");
  delete temp;
  delete tmpValues;

  temp = new char[strlen(sqlQuery) + strlen(values) + 10];
  
  strcpy(temp,sqlQuery);
  strcat(temp,values);
  delete sqlQuery;
  delete values;
  
  *sql = temp;
  return 0;


}
*/
/*!function for storing data to a SQLITE LASTVALUE type table
*/
/*
int DBTriggersTable::updateSqlite(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;

  int first = 1;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("UPDATE ") + strlen(parameters.tbName) + strlen(" SET ")+5];
  strcpy(temp,"UPDATE ");
  strcat(temp,parameters.tbName);
  strcat(temp," SET ");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);

  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      temp = new char[strlen(sqlQuery)+5];
      strcpy(temp,sqlQuery);
      delete sqlQuery;
      
      if(parameters.stField[i].isValid)
	{
	  if(!first)
	      strcat(temp,",");
	  first = 0;
	  
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      field = new char[parameters.stField[i].iValue +5];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+5 + strlen(field)];
	  strcpy(sqlQuery,temp);
	  strcat(sqlQuery,parameters.stField[i].name);
	  strcat(sqlQuery,"=");
	  strcat(sqlQuery,field);

	  delete field;
	}
      else
	{
	  sqlQuery = new char[strlen(temp)+5];
	  strcpy(sqlQuery,temp);
	}
    }
  delete temp;

  *sql = sqlQuery;
  return 0;
}
*/
/*!function for store data to a MySQL table
*/
/*
int DBTriggersTable::storeMysql(char **sql)
{
  char *sqlQuery = NULL;
  int ret = -1;

  sqlQuery = *sql;
  if(!strcmp(parameters.tbType,"LASTVALUE"))
    ret = updateMysql(&sqlQuery);
  else
    ret = insertMysql(&sqlQuery);
  
  *sql = sqlQuery;
}
*/
/*!function for insert data to a MySQL LOG type table
*/
/*
int DBTriggersTable::insertMysql(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;

  char *values;
  char *tmpValues;
  int first = 1;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("INSERT INTO ") + strlen(parameters.tbName) + strlen(" (") + 7];
  strcpy(temp,"INSERT INTO ");
  strcat(temp,"`");
  strcat(temp,parameters.tbName);
  strcat(temp,"`(");

  tmpValues = new char[strlen("VALUES (") +5];
  strcpy(tmpValues,"VALUES (");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);
  values =  new char[strlen(tmpValues)+5];
  strcpy(values,tmpValues);
  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      delete tmpValues;
      temp = new char[strlen(sqlQuery)+5];
      tmpValues = new char[strlen(values)+5];
      strcpy(temp,sqlQuery);
      strcpy(tmpValues,values);
      delete values;
      delete sqlQuery;
      
      if(parameters.stField[i].isValid)
	{
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      field = new char[parameters.stField[i].iValue +5];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+7];
	  values = new char[strlen(tmpValues)+strlen(field)+5];
	  strcpy(sqlQuery,temp);
	  strcpy(values,tmpValues);
	  if(!first)
	    {
	      strcat(values,",");
	      strcat(sqlQuery,",");
	    }
	  else
	    first = 0;
	  strcat(values,field);
	  strcat(sqlQuery,"`");
	  strcat(sqlQuery,parameters.stField[i].name);
	  strcat(sqlQuery,"`");
	  delete field;
	}
      else
	{
	  sqlQuery = new char[strlen(temp)+5];
	  values = new char[strlen(tmpValues)+5];
	  strcpy(sqlQuery,temp);
	  strcpy(values,tmpValues);
	}
    }
  strcat(sqlQuery,") ");
  strcat(values,") ");
  delete temp;
  delete tmpValues;

  temp = new char[strlen(sqlQuery) + strlen(values) + 10];
  
  strcpy(temp,sqlQuery);
  strcat(temp,values);
  delete sqlQuery;
  delete values;
  
  *sql = temp;
  return 0;
}
*/
/*!function for storing data to a MySQL LASTVALUE type table
*/
/*
int DBTriggersTable::updateMysql(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;

  int first = 1;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("UPDATE ") + strlen(parameters.tbName) + strlen(" SET ")+5];
  strcpy(temp,"UPDATE ");
  
  strcat(temp,"`");
  strcat(temp,parameters.tbName);
  strcat(temp,"` SET ");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);

  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      temp = new char[strlen(sqlQuery)+5];
      strcpy(temp,sqlQuery);
      delete sqlQuery;
      
      if(parameters.stField[i].isValid)
	{
	  if(!first)
	      strcat(temp,",");
	  first = 0;
	  
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      field = new char[parameters.stField[i].iValue +5];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+5 + strlen(field)];
	  strcpy(sqlQuery,temp);
	  strcat(sqlQuery,"`");
	  strcat(sqlQuery,parameters.stField[i].name);
	  strcat(sqlQuery,"`=");
	  strcat(sqlQuery,field);

	  delete field;
	}
      else
	{
	  sqlQuery = new char[strlen(temp)+5];
	  strcpy(sqlQuery,temp);
	}
    }
  delete temp;

  *sql = sqlQuery;
  return 0;
}
*/
/*!funtion for retunring a sql command to take every trigger triggered in database table*/
int DBTriggersTable::sqlTgsTgd(char * & sql)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::sqlTgsTgd)" << std::endl;
  sql = new char[strlen("SELECT `NAME` from `") + strlen(parameters.tbName) + strlen("` where (`VALUE`= 1)")+5];
  sprintf(sql,"SELECT `NAME` from `%s` where (`VALUE`= 1)", parameters.tbName);

  return 0;
}
/*!function for setting on triggers*/
int DBTriggersTable::updateTriggersOn(char **triggers, int numberOf)
{
  //std::cout << "DEBUG: (inside DBTriggersTable::updateTriggersOn)" << std::endl; 
  for(int i = 0; i < numberOf;i++)
    {
      for(int j=0; j < parameters.numFields; j++)
	{
	  if(!strcmp(triggers[i], parameters.stField[j].name))
	    {
	      parameters.stField[j].iValue = 1;
	      parameters.stField[j].isValid = 1;	      
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
