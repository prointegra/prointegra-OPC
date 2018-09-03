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
@File piDataTable.cpp
'''Data table class
creates it dinamically and store retrieve data'''
*/

#include "piDataTable.h"


/*!function for creating database tables dinamycally
TODO: only implemented sqlite,MySQL tables!
*/
int DBDataTable::create(databaseParameters* dbParameters,int* nQueries, char ***query)
{
  char **sqlQuery = NULL;
  int ret=-1;

  sqlQuery = *query;

  //std::cout << "DEBUG: (inside DBDataTable::create) asigning 1 to *nQueries "<< std::endl;
  *nQueries = 1;
  //std::cout << "DEBUG: (inside DBDataTable::create) creating an array of:asigning values to *nQueries =" << *nQueries << std::endl;
  if(!strcmp(parameters.tbType,"LASTVALUE"))
      *nQueries = 3;
  //std::cout << "DEBUG: (inside DBDataTable::create) creating an array of:" << *nQueries << " SQL queries" << std::endl;
  sqlQuery = new char*[*nQueries];
  //std::cout << "DEBUG: (inside DBTable::create) created an array of:"<< *num << " SQL queries" << std::endl;
  for(int i=0;i<*nQueries;i++)
    sqlQuery[i]=NULL;
  
  if(!strcmp(dbParameters->type,"QSQLITE"))
    {
      //std::cout << "DEBUG: (inside DBTable::create) creating SQLITE quieres" << std::endl;
      creationSqlite(&sqlQuery[0]);
      if(*nQueries>1)
	initValuesSqlite(*nQueries,&sqlQuery);
      ret = 0;
    }
   else if(!strcmp(dbParameters->type,"QMYSQL"))
    {
      //std::cout << "DEBUG: (inside DBTable::create) creating MySQL queries" << std::endl;
      creationMysql(&sqlQuery[0]);
      if(*nQueries>1)
	initValuesMysql(*nQueries,&sqlQuery);
      ret = 0;
    }
  if(ret)
    {
      delete sqlQuery;
      *nQueries = 0;
    }
  //std::cout << "DEBUG: (inside DBTable::create) copying back pointers, ret:" << ret << std::endl;
  *query =  sqlQuery;
  //std::cout << "DEBUG: (inside DBTable::create) returning ret:" << ret << std::endl;
  return ret;
}

/*!function for creating the database schema, for SQLite databases*/
int DBDataTable::creationSqlite(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("DROP TABLE IF EXISTS `") + strlen(parameters.tbName) + strlen("` ; CREATE TABLE ") + strlen(parameters.tbName) + strlen(" (ID INTEGER PRIMARY KEY AUTOINCREMENT,")+5];
  strcpy(temp,"DROP TABLE IF EXISTS ");
  strcat(temp,parameters.tbName);
  strcat(temp," ; CREATE TABLE ");
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
int DBDataTable::creationMysql(char **sql)
{
  char *sqlQuery = NULL;
  char * temp = NULL;
  char * field = NULL;
  
  sqlQuery = *sql;
  
  if(sqlQuery != NULL)
    delete(sqlQuery);
  
  temp = new char[strlen("DROP TABLE IF EXISTS `") + strlen(parameters.tbName) + strlen("` ; CREATE TABLE ") + strlen(parameters.tbName) + 2 + strlen(" (ID INTEGER PRIMARY KEY AUTO_INCREMENT,")+5];
  strcpy(temp,"DROP TABLE IF EXISTS `");
  strcat(temp,parameters.tbName);
  strcat(temp,"`; CREATE TABLE `");
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
int DBDataTable::initValuesSqlite(int num,char ***sql)
{
  char **sqlQuery = NULL;
  int ret = -1;
  
  sqlQuery = *sql;
  if(num == 3 && strcmp(parameters.tbType,"LOG")) //only contemplated option
    {

      for(int i=1;i<num;i++)
	{
	  if(sqlQuery[i] != NULL)
	    delete(sqlQuery[i]);	
	}
      //std::cout << "DEBUG: (inside DBTable::initValuesSqlite) creating SQL num:1" << std::endl;
      sqlQuery[1] = new char[strlen("DELETE FROM  ") + strlen(parameters.tbName) + 7];
      sprintf(sqlQuery[1],"DELETE FROM `%s`",parameters.tbName );
      //std::cout << "DEBUG: (inside DBTable::initValuesSqlite) creating SQL num:2" << std::endl;
      sqlQuery[2] = new char[strlen("INSERT INTO ") + strlen(parameters.tbName) + strlen(" () VALUES ()")+7];
      sprintf(sqlQuery[2],"INSERT INTO `%s` () VALUES ()",parameters.tbName );
      ret = 0;
    }

  *sql = sqlQuery;
  return ret;
}
/*!function for insert initialziation NULL data to a MySQL NOT LOG type table
*/
int DBDataTable::initValuesMysql(int num,char ***sql)
{
  char **sqlQuery = NULL;
  int ret = -1;
  sqlQuery = *sql;

  if(num == 3 && strcmp(parameters.tbType,"LOG")) //only contemplated option
    {
      for(int i=1;i<num;i++)
	{
	  if(sqlQuery[i] != NULL)
	    delete(sqlQuery[i]);
	
	}
      //std::cout << "DEBUG: (inside DBTable::initValuesMysql) creating SQL num:1" << std::endl;
      sqlQuery[1] = new char[strlen("DELETE FROM  ") + strlen(parameters.tbName) + 7];
      sprintf(sqlQuery[1],"DELETE FROM `%s`",parameters.tbName );
      //std::cout << "DEBUG: (inside DBTable::initValuesMysql) creating SQL num:2" << std::endl;
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
int DBDataTable::store(databaseParameters* parameters,char **query)
{
  char *sqlQuery = NULL;
  int ret;

  sqlQuery = *query;
  
  if(!strcmp(parameters->type,"QSQLITE"))
    {
      ret = storeSqlite(&sqlQuery);
    }
  else if(!strcmp(parameters->type,"QMYSQL"))
    {
      ret = storeMysql(&sqlQuery);
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
int DBDataTable::storeSqlite(char **sql)
{
  char *sqlQuery = NULL;
  int ret = -1;

  sqlQuery = *sql;
  if(!strcmp(parameters.tbType,"LASTVALUE"))
    ret = updateSqlite(&sqlQuery);
  else
    ret = insertSqlite(&sqlQuery);
  
  *sql = sqlQuery;
  return ret;
}

/*!function for inserting data to a sqlite table
*/
int DBDataTable::insertSqlite(char **sql)
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
	  //TODO TO IMPROVE, WHAT IF DIGITAL?¿? OR OTHERS
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      /*BUGFIX STD::BAD_ALLOC TODO-> TO IMPROVE*/
	      //field = new char[parameters.stField[i].iValue +5];
	      field = new char[16];
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
int DBDataTable::updateSqlite(char **sql)
{
  int failed = -1;
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
	  //TODO TO IMPROVE, WHAT IF DIGITAL?¿? OR OTHERS
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      /*BUGFIX STD::BAD_ALLOC TODO-> TO IMPROVE*/
	      //field = new char[parameters.stField[i].iValue +5];
	      field = new char[16];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+5 + strlen(field)];
	  strcpy(sqlQuery,temp);
	  strcat(sqlQuery,parameters.stField[i].name);
	  strcat(sqlQuery,"=");
	  strcat(sqlQuery,field);
	  failed = 0;

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
  return failed;
}
/*!function for store data to a MySQL table
*/
int DBDataTable::storeMysql(char **sql)
{
  char *sqlQuery = NULL;
  int ret = -1;

  sqlQuery = *sql;
  if(!strcmp(parameters.tbType,"LASTVALUE"))
    ret = updateMysql(&sqlQuery);
  else
    ret = insertMysql(&sqlQuery);
  
  *sql = sqlQuery;
  return ret;
}
/*!function for insert data to a MySQL LOG type table
*/
int DBDataTable::insertMysql(char **sql)
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
	  //TODO TO IMPROVE, WHAT IF DIGITAL?¿? OR OTHERS
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      /*BUGFIX STD::BAD_ALLOC TODO-> TO IMPROVE*/
	      //field = new char[parameters.stField[i].iValue +5];
	      field = new char[16];
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
int DBDataTable::updateMysql(char **sql)
{
  int failed = -1;
  
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
	  //TODO TO IMPROVE, WHAT IF DIGITAL?¿? OR OTHERS
	  if(!strcmp(parameters.stField[i].type,"INT")||!strcmp(parameters.stField[i].type,"FLOAT"))
	    {
	      /*BUGFIX STD::BAD_ALLOC TODO-> TO IMPROVE*/
	      //field = new char[parameters.stField[i].iValue +5];
	      field = new char[16];
	      sprintf(field,"%d",parameters.stField[i].iValue);
	    }
	  sqlQuery = new char[strlen(temp)+strlen(parameters.stField[i].name)+5 + strlen(field)];
	  strcpy(sqlQuery,temp);
	  strcat(sqlQuery,"`");
	  strcat(sqlQuery,parameters.stField[i].name);
	  strcat(sqlQuery,"`=");
	  strcat(sqlQuery,field);
	  failed = 0;

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
  return failed;
}

//RETURNING DATA FUNCTIONS
/*!function to return a if table reading is trigger drived*/
int DBDataTable::isTimeTriggered()
{
  int ret = 0;
  if(parameters.tbTriggerTime > 0)
    {
      if(difftime(time(0), read) >= parameters.tbTriggerTime)
	{
	  ret = 1;
	}
    }
  
  return ret;
}
/*!function to return if time trigger is initialized*/
int DBDataTable::isTimeInitialized()
{
  //std::cout << "DEBUG: (inside DBDataTable::isTimeInitialized)" << std::endl;
  int ret = 1;
  if (parameters.tbTrigger > 0)
    {
      if(read == NULL)
	{
	  ret = 0;
	}
    }
  
  return ret;
}

/*!function to return a if table reading is trigger drived*/
int DBDataTable::isReadTriggered()
{
  int ret = 0;
  if(parameters.tbTrigger != NULL)
    ret = 1;

  return ret;
}
/*!function to return a if table writting is trigger drived*/
int DBDataTable::isWriteTriggered()
{
  int ret = 0;
  if(parameters.tbWTrigger != NULL)
    ret = 1;

  return ret;
}

/*!function to return a table reading trigger*/
int DBDataTable::retReadTrigger(field* trigger)
{
  int failed = -1;

  if (isReadTriggered())
    {
      trigger->name = new char[strlen(parameters.tbTrigger)+5];
      strcpy(trigger->name,parameters.tbTrigger);
      trigger->tag = new char[strlen(parameters.tbTrigger)+5];
      strcpy(trigger->tag,parameters.tbTrigger);
      
      trigger->type = new char[strlen("INT")+5];
      strcpy(trigger->type,"INT");
      //std::cout << "DEBUG: (inside DBDataTable::retReadTrigger) read trigger = " << trigger->name << std::endl;
      //std::cout << "DEBUG: (inside DBDataTable::retReadTrigger) PARAMETERS read trigger = " << parameters.tbTrigger << std::endl;
      failed = 0;
    }

  return failed;
}

/*!function to return a table writting trigger*/
int DBDataTable::retWriteTrigger(field* trigger)
{
  int failed = -1;

  if (isWriteTriggered())
    {
      trigger->name = new char[strlen(parameters.tbWTrigger)+5];
      strcpy(trigger->name,parameters.tbWTrigger);
      trigger->tag = new char[strlen(parameters.tbWTrigger)+5];
      strcpy(trigger->tag,parameters.tbWTrigger);
      
      trigger->type = new char[strlen("INT")+5];
      strcpy(trigger->type,"INT");
      //std::cout << "DEBUG: (inside DBDataTable::retWriteTrigger) write trigger = " << trigger->name << std::endl;
      //std::cout << "DEBUG: (inside DBDataTable::retWriteTrigger) PARAMETERS write trigger = " << parameters.tbWTrigger << std::endl;
      failed = 0;
    }

  return failed;
}



/*!function to return linking info from tag
*/
std::vector<std::vector <int>> DBDataTable::retLink(int field)
{
  //std::cout << "DEBUG:(inside DBDataTable::retLink)" << std::endl;
  std::vector<std::vector <int>> temp;
  if(field >= 0 && field < parameters.numFields)
    {
      temp = parameters.stField[field].fromTags;
    }
  
 return temp;

}

/////
//Setting data functions
/*!function to set a linking info to tag
*/
int DBDataTable::setLink(int field, int slave, int tag)
{
  //std::cout << "DEBUG: (inside DBDataTable::setLink)" << std::endl;
  int ret = -1;
  if(field >= 0 && field < parameters.numFields)
    {
      if(parameters.stField[field].fromTags.size() <=slave)
	parameters.stField[field].fromTags.resize(slave+1);
      parameters.stField[field].fromTags[slave].push_back(tag);
      ret = 0;
    }
  
 return ret;
}

/*!function to update all data
*/
int DBDataTable::updateData(std::vector <field> data)
{
  //std::cout << "DEBUG: (inside DBDataTable::updateData)" << std::endl;
  int failed = -1;

  for(int i=0; i < data.size(); i++)
    {
      for(int j=0; j < parameters.numFields;j++)
	{    
	  if(!strcmp(data.at(i).name, parameters.stField[j].name))
	    {
	      failed = 0;
	      parameters.stField[j].iValue = data.at(i).iValue;
	      parameters.stField[j].isValid = data.at(i).isValid;
	    }
	}
    }
  
 return failed;
}
