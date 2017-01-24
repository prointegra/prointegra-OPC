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
@File piTriggersTable.h
*/

#ifndef _PITRIGGERSTABLE_
#define _PITRIGGERSTABLE_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "config.h"
#include "piTable.h"

/*! Database triggers table interface class */
class DBTriggersTable : public DBTable
{
 public:
  DBTriggersTable(tableParameters tableParams) : DBTable(tableParams) {}
  ~DBTriggersTable();
  //creation
  int create(databaseParameters* dbParameters,char **query, char ***initQuery, int **nTrigs);
  int creationSqlite(char **sql);
  int creationMysql(char **sql);
  int initValuesSqlite(char ***sql, int ** nTrigs);
  int initValuesMysql(char ***sql, int ** nTrigs);
  //storing
  //int store(databaseParameters* parameters,char **query);
  //int storeSqlite(char **sql);
  //int insertSqlite(char **sql);
  //int updateSqlite(char **sql);
  //int storeMysql(char **sql);
  //int insertMysql(char **sql);
  //int updateMysql(char **sql);
  //retrieving
  int sqlTgsTgd(char *& sql);
  int sqlResetTg(char *& sql, char *triggerName);
  int updateTriggersOn(char ** triggers, int numberOf);
 
  //return private members
  int retNumFields(){return parameters.numFields;};
  char* retFieldTag(int field);
  int retFieldValid(int field);
  int retFieldValue(int field);
  int* retLink(int field);
  int retNoRepeatedFields(char***);
  int retTgsTgd(field ***, int **);
  //set attributes
  int setFieldValid(int field, int valid);
  int setFieldValue(int field, int value);
  int setLink(int field, int slave, int tag);
  //debug
  int showTriggers();

};


#endif
