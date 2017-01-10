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
  int create(databaseParameters* parameters,int ** nQueries,char ***query);
  int creationSqlite(char **sql);
  int creationMysql(char **sql);
  int initValuesSqlite(int num,char ***sql);
  int initValuesMysql(int num,char ***sql);
  //storing
  int store(databaseParameters* parameters,char **query);
  int storeSqlite(char **sql);
  int insertSqlite(char **sql);
  int updateSqlite(char **sql);
  int storeMysql(char **sql);
  int insertMysql(char **sql);
  int updateMysql(char **sql); 
  //return private members
  int retNumFields(){return parameters.numFields;};
  char * retFieldTag(int field);
  int retFieldValid(int field);
  int retFieldValue(int field);
  int* retLink(int field);
  //set attributes
  int setFieldValid(int field, int valid);
  int setFieldValue(int field, int value);
  int setLink(int field, int slave, int tag);
  
private:  
  tableParameters parameters;
  
};



#endif
