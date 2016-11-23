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
'''personalize this files for custom data structures'''
*/

#include "piDatabase.h"


int DBInterface::start()
{
  //SQLSERVER EXAMPLE
  //if(database->open("QTDS7","PROINTEG-3FCFDD","master","sa","prointegra"))
  //SQLITE 
  if(qtDatabase::open(conParameters.type,conParameters.host,conParameters.dbName,conParameters.user,conParameters.pass))
    {
      std::cout << "DEBUG:ERROR OPENING DATABASE " << std::endl;
    }
}


//check tables and create them if not exist
//personalize!
int DBInterface::checkAndCreate()
{
  //table "recepcion"
  //index,date,time,silo de recepcion INT, tipo de producto STRING, kilos INT, matricula del camion STRING,proveedor STRING, vacio1 STRING, vacio2 STRING, vacio3 STRING, vacio4 STRING, vacio5 STRING;
  //
  //table "proceso"
  //index, date, time, silo recepcion INT, silo limpia INT, tipo de producto STRING
  //
  //table "salida"
  //index, date, time, juliano envasado INT, juliano limpia INT, silo limpia INT, tipo de producto STRING

  int ret;
  char       buf[80];
 
  std::cout << "DEBUG: Creating table \"recepcion\"" << std::endl;
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS recepcion(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,SILO_RECEPCION INT,TIPO_DE_PRODUCTO INT,KILOS INT, MATRICULA TEXT,PROVEEDOR INT,VACIO1 INT,VACIO2 INT,VACIO3 INT,VACIO4 INT,VACIO5 INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE recepcion" << std::endl;
    }
  std::cout << "DEBUG: Creating table \"proceso\"" << std::endl;
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS proceso(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,SILO_RECEPCION INT,SILO_LIMPIA INT, TIPO_DE_PRODUCTO INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE proceso" << std::endl;
    }
  //
    std::cout << "DEBUG: Creating table \"julianos\"" << std::endl;
    ret = query(NULL,"CREATE TABLE IF NOT EXISTS julianos(SILO INT NOT NULL UNIQUE,JULIANO INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
   ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(5,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(6,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(7,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    }
     ret = query(NULL,"INSERT OR IGNORE INTO julianos VALUES(8,0)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE julianos" << std::endl;
    } 
  //julian test
  //time_t     now;
  //now = time(0);
  //struct tm *ts;
  //ts = localtime(&now);

  //strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
  //std::cout << "fecha original: " << buf << "   ,fecha juliana: " << gstTime2JulianI(&ts) << std::endl;
  std::cout << "DEBUG: Creating table \"producto\"" << std::endl;
  
  ret = query(NULL,"CREATE TABLE IF NOT EXISTS producto(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATE,HORA TIME,JULIANO_ENVASADO INT,JULIANO_LIMPIA INT, TIPO_DE_PRODUCTO INT, SILO_LIMPIA INT,KILOS INT)");
  if(ret != 0)
    {
      std::cout << "ERROR CREATING TABLE producto" << std::endl;
    }

  return 0;
}

//return juliano from tabe julianos
int DBInterface::getJuliano(int silo)
{
  char sql[500];
  int ret;
  char resultado[500];
  QSqlDatabase localdb = *db;

  sprintf(sql,"SELECT juliano from julianos where (silo = %d)",silo);

  // std::cout << sql  << std::endl;

  QSqlQuery query(sql,localdb);
  int idName = query.record().indexOf("juliano");
  sprintf(resultado,query.value(idName).toString().toUtf8());
  std::cout << resultado << std::endl;
  query.first();
  while (query.next())
    {
      sprintf(resultado,query.value(idName).toString().toUtf8());
      std::cout << resultado << std::endl;
    }

  /* ret = query(NULL,sql);
  if(ret != 0)
    {
      std::cout << "ERROR retrieving juliano from database" << std::endl;
    }
  std::cout << result.numRowsAffected() << std::endl;
  result->last();
  std::cout << result->last()+1 << std::endl; 
  //  std::cout << "juliano :" <<  recordFieldValue(NULL, 0) << std::endl;
  std::cout << (const char *) result->record().fieldName(0).toUtf8() << std::endl;
  result->next();
  std::cout << (const char *) result->record().fieldName(0).toUtf8() << std::endl; 
  */
  return 1;
  
}

///TABLE FUNCTIONS
DBTable::DBTable()
{

  return;
}
DBTable::~DBTable()
{

  return;
}


