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
  int store(databaseParameters* parameters,char **query);
  int storeSqlite(char **sql);
  int insertSqlite(char **sql);
  int updateSqlite(char **sql);
  int storeMysql(char **sql);
  int insertMysql(char **sql);
  int updateMysql(char **sql);
  //new sql standard functions, january 2017, TODO: to improve and insert the old ones
  int sqlSelectAll(databaseParameters dbParameters,char* & sql);
  int sqlSelectAllMysql(char* & sql);
  int sqlSelectAllSqlite(char* & sql);
  
  //return private members
  int retNumFields(){return parameters.numFields;};
  char * retFieldTag(int field);
  int retFieldValid(int field);
  int retFieldValue(int field);
  int* retLink(int field);
  int retFields(field ***,int**);
  //set attributes
  int setFieldValid(int field, int valid);
  int setFieldValue(int field, int value);
  int setAllValues(char ***table, int columns, int rows, int skip);
  int setLink(int field, int slave, int tag);
  
protected:  
  tableParameters parameters;
  
};



#endif
