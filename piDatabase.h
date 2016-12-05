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
@File piDatabase.h
*/

#ifndef _PIDATABASE_
#define _PIDATABASE_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "qtdatabase.h"
#include "config.h"


/*! Database table interface class */
class DBTable
{
 public:
  DBTable(tableParameters tableParams);
  ~DBTable();

  int create(databaseParameters* parameters,char **query);
  int creationSqlite(char **sql);
  int store(databaseParameters* parameters,char **query);
  int storeSqlite(char **sql);
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

/*! Database interface class, derived from pvbrowser addons examples */
class DBInterface : public qtDatabase
{
 public:
 DBInterface() : qtDatabase(){
  };
  int setup(databaseParameters dbParams,tableParameters* tablesParams);
  int start();
  //sql functions
  int storeData();
  //return private members
  int retNumTables(){return parameters.numTables;};
  int retNumFields(int table);
  char * retFieldTag(int table,int field);
  int retFieldValid(int table,int field);
  int retFieldValue(int table,int field);
  //set attributes
  int setFieldValid(int table,int field, int valid);
  int setFieldValue(int table,int field, int value);
  //linking fields with communications
  int fieldLinked(int table,int field);
  int* retFieldLink(int table, int field);
  int fieldLink(int table, int field, int slave, int tag);
  
 private:
  databaseParameters parameters;
  DBTable** tables;
};




#endif
