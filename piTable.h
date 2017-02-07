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
@File piTable.h
*/

#ifndef _PITABLE_
#define _PITABLE_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "config.h"


/*! Database table interface class */
class DBTable
{
 public:
  DBTable(tableParameters tableParams);
  ~DBTable();
  //creation
  virtual int create(databaseParameters* parameters,int ** nQueries,char ***query);
  virtual int creationSqlite(char **sql);
  virtual int creationMysql(char **sql);
  virtual int initValuesSqlite(int num,char ***sql);
  virtual int initValuesMysql(int num,char ***sql);
  //storing
  virtual int store(databaseParameters* parameters,char **query);
  virtual int storeSqlite(char **sql);
  virtual int insertSqlite(char **sql);
  virtual int updateSqlite(char **sql);
  virtual int storeMysql(char **sql);
  virtual int insertMysql(char **sql);
  virtual int updateMysql(char **sql);
  //new sql standard functions, january 2017, TODO: to improve and insert the old ones
  virtual int sqlSelectAll(databaseParameters dbParameters,char* & sql);
  virtual int sqlSelectAllMysql(char* & sql);
  virtual int sqlSelectAllSqlite(char* & sql);
  
  //return private members
  int retNumFields(){return parameters.numFields;};
  char * retFieldTag(int field);
  int retFieldValid(int field);
  int retFieldValue(int field);
  int retFields(field ***,int**);
  int retvFields(std::vector < field> & fields);
  int retId(){return parameters.id;};
  
  //set attributes
  int setFieldValid(int field, int valid);
  int setFieldValue(int field, int value);
  int setAllValues(char ***table, int columns, int rows, int skip);
  int setId(int id);
  
protected:  
  tableParameters parameters;
  
};



#endif
