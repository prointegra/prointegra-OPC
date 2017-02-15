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
@File piTable.cpp
'''Generic DB table interface'''
*/

#include "piTable.h"

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
/*!function for creating database tables dinamycally
TODO: only implemented sqlite,MySQL tables!
*/
int DBTable::create(databaseParameters* dbParameters,int** nQueries, char ***query)
{
  char **sqlQuery = NULL;
  int ret=-1;
  int* num;

  sqlQuery = *query;
  num = *nQueries;
  std::cout << "DEBUG: (inside DBTable::create) asigning 0 to *num "<< std::endl;
  *num = 0;
  std::cout << "DEBUG: (inside DBTable::create) creating an array of:asigning values to *num =" << *num << std::endl;
  if(!strcmp(parameters.tbType,"LASTVALUE") || !strcmp(parameters.tbType,"TRIGGERS"))
    {
      *num = 3;
    }
  else
    *num=1;
  std::cout << "DEBUG: (inside DBTable::create) creating an array of:" << *num << " SQL queries" << std::endl;
  sqlQuery = new char*[*num];
  std::cout << "DEBUG: (inside DBTable::create) created an array of:"<< *num << " SQL queries" << std::endl;
  for(int i=0;i<*num;i++)
    sqlQuery[i]=NULL;
  
  if(!strcmp(dbParameters->type,"QSQLITE"))
    {
      std::cout << "DEBUG: (inside DBTable::create) creating SQLITE queries" << std::endl;
      creationSqlite(&sqlQuery[0]);
      if(*num>1)
	initValuesSqlite(*num,&sqlQuery);
      ret = 0;
    }
   else if(!strcmp(dbParameters->type,"QMYSQL"))
    {
      std::cout << "DEBUG: (inside DBTable::create) creating MySQL queries" << std::endl;
      creationMysql(&sqlQuery[0]);
      if(*num>1)
	initValuesMysql(*num,&sqlQuery);
      ret = 0;
    }
  if(ret)
    {
      delete sqlQuery;
      *num = 0;
    }
  std::cout << "DEBUG: (inside DBTable::create) copying back pointers, ret:" << ret << std::endl;
  *nQueries = num;
  *query =  sqlQuery;
  std::cout << "DEBUG: (inside DBTable::create) returning ret:" << ret << std::endl;
  return ret;
}
/*!function for creating the database schema, for SQLite databases*/
int DBTable::creationSqlite(char **sql)
{
  static char *sqlQuery = NULL;
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
/*!function for creating the database schema, for MySQL databases*/
int DBTable::creationMysql(char **sql)
{
  static char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("CREATE TABLE IF NOT EXISTS ") + strlen(parameters.tbName) + 2 + strlen(" (ID INTEGER PRIMARY KEY AUTO_INCREMENT,")+5];
  strcpy(temp,"CREATE TABLE IF NOT EXISTS `");
  strcat(temp,parameters.tbName);
  strcat(temp,"`");
  strcat(temp," (ID INTEGER PRIMARY KEY AUTO_INCREMENT,");

  sqlQuery = new char[strlen(temp)+5];
  strcpy(sqlQuery,temp);
  for (int i = 0; i < parameters.numFields;i++)
    {
      delete temp;
      temp = new char[strlen(sqlQuery)+5];
      strcpy(temp,sqlQuery);
      delete sqlQuery;
	
      field = new char[strlen(parameters.stField[i].type) + strlen(parameters.stField[i].name) + 7];
      strcpy(field,"`");
      strcat(field,parameters.stField[i].name);
      strcat(field,"`");
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
/*!function for insert initialization NULL data to a SQLITE NOT LOG type table
*/
int DBTable::initValuesSqlite(int num,char ***sql)
{
  static char **sqlQuery = NULL;
  int ret = -1;
  
  sqlQuery = *sql;
  if(num == 3 && strcmp(parameters.tbType,"LOG")) //only contemplated option
    {

      for(int i=1;i<num;i++)
	{
	  if(sqlQuery[i] != NULL)
	    delete(sqlQuery[i]);	
	}
      std::cout << "DEBUG: (inside DBTable::initValuesSqlite) creating SQL num:1" << std::endl;
      sqlQuery[1] = new char[strlen("DELETE FROM  ") + strlen(parameters.tbName) + 7];
      sprintf(sqlQuery[1],"DELETE FROM `%s`",parameters.tbName );
      std::cout << "DEBUG: (inside DBTable::initValuesSqlite) creating SQL num:2" << std::endl;
      sqlQuery[2] = new char[strlen("INSERT INTO ") + strlen(parameters.tbName) + strlen(" () VALUES ()")+7];
      sprintf(sqlQuery[2],"INSERT INTO `%s` () VALUES ()",parameters.tbName );
      ret = 0;
    }

  *sql = sqlQuery;
  return ret;
}
/*!function for insert initialziation NULL data to a MySQL NOT LOG type table
*/
int DBTable::initValuesMysql(int num,char ***sql)
{
  static char **sqlQuery = NULL;
  int ret = -1;
  sqlQuery = *sql;

  if(num == 3 && strcmp(parameters.tbType,"LOG")) //only contemplated option
    {
      for(int i=1;i<num;i++)
	{
	  if(sqlQuery[i] != NULL)
	    delete(sqlQuery[i]);
	
	}
      std::cout << "DEBUG: (inside DBTable::initValuesMysql) creating SQL num:1" << std::endl;
      sqlQuery[1] = new char[strlen("DELETE FROM  ") + strlen(parameters.tbName) + 7];
      sprintf(sqlQuery[1],"DELETE FROM `%s`",parameters.tbName );
      std::cout << "DEBUG: (inside DBTable::initValuesMysql) creating SQL num:2" << std::endl;
      sqlQuery[2] = new char[strlen("INSERT INTO ") + strlen(parameters.tbName) + strlen(" () VALUES ()")+7];
      sprintf(sqlQuery[2],"INSERT INTO `%s` () VALUES ()",parameters.tbName );
      ret = 0;
    }

  *sql = sqlQuery;
  return ret;
}

/*!function for store data to the table
TODO: only implemented sqlite,MySQL tables!
*/
int DBTable::store(databaseParameters* parameters,char **query)
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

/*!function for storing data to a SQLITE table
*/
int DBTable::storeSqlite(char **sql)
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

/*!function for inserting data to a sqlite table
*/
int DBTable::insertSqlite(char **sql)
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

/*!function for storing data to a SQLITE LASTVALUE type table
*/
int DBTable::updateSqlite(char **sql)
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
/*!function for store data to a MySQL table
*/
int DBTable::storeMysql(char **sql)
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
/*!function for insert data to a MySQL LOG type table
*/
int DBTable::insertMysql(char **sql)
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
/*!function for storing data to a MySQL LASTVALUE type table
*/
int DBTable::updateMysql(char **sql)
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
/*sql functions TODO: to fill with standard sql functions*/
/*!select all from table*/
int DBTable::sqlSelectAll(databaseParameters dbParameters,char* & sql)
{
  int failed = -1;
  if(!strcmp(dbParameters.type,"QSQLITE"))
    {
      failed = sqlSelectAllSqlite(sql);
    }
  else if(!strcmp(dbParameters.type,"QMYSQL"))
    {
      failed = sqlSelectAllMysql(sql);
    }
  return failed;
}

/*!select all from MySQL table*/
int DBTable::sqlSelectAllMysql(char* & sql)
{
  int failed = -1;
  sql = new char[strlen("SELECT * FROM `") + strlen(parameters.tbName) + strlen("`") +5];
  sprintf(sql,"SELECT * FROM `%s`",parameters.tbName);
  failed = 0;
  return failed;
}
/*!select all from SQLITE table*/
int DBTable::sqlSelectAllSqlite(char* & sql)
{
  int failed = -1;
  sql = new char[strlen("SELECT * FROM ") + strlen(parameters.tbName) +5];
  sprintf(sql,"SELECT * FROM %s",parameters.tbName);
  failed = 0;
  return failed;
}

/*!function to return a field tag
TODO: it should, for convention, only return once*/
char * DBTable::retFieldTag(int field)
{
  if(field >= 0 && field < parameters.numFields)
    return parameters.stField[field].tag;
  else
    return NULL;

}

/*!function to return a field valid variable*/
int DBTable::retFieldValid(int field)
{
  int ret = -1;
  if(field >= 0 && field < parameters.numFields)
    {
      ret = parameters.stField[field].isValid;
    }
  return ret;
}

/*!function to return a field number value variable*/
int DBTable::retFieldValue(int field)
{
  int ret = -1;  
  if(field >= 0 && field < parameters.numFields)
    {
      ret = parameters.stField[field].iValue;      
    }
  return ret;
}

/*!function to return the complete table field list*/
int DBTable::retFields(field *** fields, int ** numberOf)
{
  //std::cout << "DEBUG: (inside DBTable::retFields)" << std::endl;
  int failed = -1;
  static field ** stFields;
  static int * numFields;

  stFields = *fields;
  numFields = *numberOf;

  numFields = new int(0);
  *numFields = parameters.numFields;
  stFields = new field*[*numFields];
  for(int i=0; i < *numFields; i++)
      stFields[i] = new field(parameters.stField[i]);

  *numberOf = numFields;
  *fields = stFields;
  return 0;
}

/*!function to return the complete table field list in a C++ vector type format*/
int DBTable::retvFields(std::vector < field> & fields)
{
  //std::cout << "DEBUG: (inside DBTable::retFields)" << std::endl;
  int failed = -1;

  fields.clear();
  for(int i=0; i < parameters.numFields; i++)
    fields.push_back(parameters.stField[i]);

  return 0;
}

//
/*!function to set a field valid variable*/
int DBTable::setFieldValid(int field, int valid)
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
int DBTable::setFieldValue(int field, int value)
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
/*!function to set a all values from table*/
int DBTable::setAllValues(char ***table,int columns, int rows, int skip)
{
  int failed = -1;
  //sanity checks
  if(rows > 0 && columns > skip)
    {
      if(columns-skip == parameters.numFields)
	{
	  for(int i = skip; i < columns; i++)
	    {
	      if(strcmp(table[rows-1][i],"NULL"))
		 parameters.stField[i-skip].iValue=atoi(table[rows-1][i]);
	    }
	  failed = 0;
	}
    }
  
  return failed;
}

/*!function to set an identification number to table*/
int DBTable::setId(int id)
{
  int failed = -1;
  //sanity checks
  if(id >= 0)
    {
      parameters.id = id;
      failed = 0;
    }
  
  return failed;
}
