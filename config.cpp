/*
 * 	Prointegra OPC
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
/**
@file config.c
*/

#include "config.h"
//constants
const char* sVERSION = "v0.0.1 ALPHA";

/*! Constructor*/
configParser::configParser(char* path)
{
  //initialization of file
  pugi::xml_parse_result result = doc.load_file(path); 
  cout << "Load result: " << result.description()  << endl;
  return;   
}
/*! Destructor*/
configParser::~configParser()
{
  if (databaseParams != NULL)
    delete databaseParams;
       
  return;   
}

/*!retrieving database parameters*/
int configParser::retrieveDBParams()
{
  int i=0;
  int nTables = 0;
  int size;
  char *type = NULL;
  char *dbName = NULL;
  char *name = NULL;
  int saveMode;
  int numTables;
  char *tableName = NULL;
  char *user = NULL;
  char * password = NULL;
  char *hostname = NULL;
  
  //1 number of databases
  for (pugi::xml_node db = doc.child("db"); db; db = db.next_sibling("db"))
    {
      i++;
    }
  //number of DB instances defined
  nDBs = i;
  ////
  databaseParams = new databaseParameters[nDBs];
  tablesParams = new tableParameters*[nDBs];
  i = 0;
  //capturing data from databases;
  for (pugi::xml_node db = doc.child("db"); db; db = db.next_sibling("db"))
    {
      //retrieve database name (name of whole entity, database name + hostname +user + password + qtdriver + number of tables...)
      retrieveCharAttr(&db,&name,"name");
      //retrieve database qt driver
      retrieveCharAttr(&db,&type,"qtdriver");
      //retrieve database hostname
      retrieveCharAttr(&db,&hostname,"hostname");
      //retrieve database id name
      retrieveCharAttr(&db,&dbName,"dbName");    
      //retrieve database user
      retrieveCharAttr(&db,&user,"user");
      //retrieve database id name user password
      retrieveCharAttr(&db,&password,"password");
      //retrieve database id name user password
      retrieveIntAttr(&db,&nTables,"tables");
      //if everything's allright
      if(!checkDBParams(name,type,hostname,dbName,user,password,nTables))
	{
	  databaseParams[i] = {  type,hostname,dbName,name,user,password,nTables };
	  tablesParams[i]= new tableParameters;
	  retrieveTableAttrs(&db,&tablesParams[i],nTables);
	  i++;
	}
    }
  //i = number of databases created 
  return 0;
}

int configParser::checkDBParams(const char* name, const char* type, const char* hostname, const char* dbName, const char* user, const char* password, int nTables)
{
  if (name != NULL)
    {
      if(!checkDBType(type))
	{
	  //if(!checkDBHostname(type,hostname, user, password))
	  //  {
	  //    if(!checkDBName(type,dbName)
	  //	{
		  if(nTables>0)
		    {
		      return 0;
		    }
		  //	}
		//}
	}
    }
  return -1;
}

int configParser::retrieveCharAttr(pugi::xml_node* db, char** name, const char* attribute)
{
  char *newName;
  int size=0;

  newName = *name;
  //better strlen than sizeof, sizeof return always 8
  if( strlen(db->attribute(attribute).value()) >0)
    {
      size =strlen(db->attribute(attribute).value()) + 1;
      newName = new char[size];
      strcpy(newName,db->attribute(attribute).value());
    }
  else
    newName = NULL;

  *name = newName;
  return 0;  
}

int configParser::retrieveIntAttr(pugi::xml_node* db, int* value, const char* attribute)
{

  int size=0;

  //as_int() suppose to return 0  if value is empty
  *value = db->attribute(attribute).as_int();

  return 0;  
}

int configParser::checkDBType(const char* type)
{
  char * temp;

  temp = new char[strlen(type)+1];
  strcpy(temp,type);
  for(int i=0;i<strlen(temp);i++)
    temp[i] = toupper(temp[i]);
  if(!strcmp(temp,"QSQLITE") || !strcmp(temp,"QTDS"))
    {
      delete temp;
      return 0;
    }
  cout << "ERROR: DATABASE " << temp << " DOESN'T EXIST OR IS NOT IMPLEMENTED" << endl;
  return -1;
}


databaseParameters configParser::retDBParams(int database)
{

  char *type = NULL;
  char *dbName = NULL;
  char *name = NULL;
  int saveMode;
  int numTables;
  char *tableName = NULL;
  char *user = NULL;
  char * password = NULL;
  char *hostname = NULL;

  databaseParameters temp = { type, hostname, dbName, name, user, password, 0 };
  
  if(database < nDBs && database >= 0)
    {
      return databaseParams[database];
    }
  else
    return temp;
}

int configParser::retrieveTableAttrs(pugi::xml_node* db, tableParameters**,int nTables)
{

  return 0;
}

/*!iterating data*/
/*
int configParser::IterateComms()
{
  char buffer[100];

  //how many devices?
  int i=0,j=0;
  for (pugi::xml_node comm = doc.child("comm"); comm; comm = comm.next_sibling("comm"))
    {
      i++;
    }
  ConfigSlaves = new mbSlaves[i];
  nSlaves = i;
  //configure device
  i=0;
  setModbusRTU(0);
  setModbusTCP(0);
  for (pugi::xml_node comm = doc.child("comm"); comm; comm = comm.next_sibling("comm"))
    {
      //slave id
      ConfigSlaves[i].slaveId = comm.attribute("slave").as_int();
      //communications type
      if(!strcmp(comm.attribute("mailbox").value(),"modbus_rtu"))
	{
	  ConfigSlaves[i].commType = 1;
	}
      if(!strcmp(comm.attribute("mailbox").value(),"modbus_tcp"))
	{
	  ConfigSlaves[i].commType = 2;
	}
      //save by trigger or by rate time?
      ConfigSlaves[i].mTime = comm.attribute("mTime").as_int();
      //registers number
      ConfigSlaves[i].nRegs = comm.attribute("registers").as_int(); 
      ConfigSlaves[i].Registers = new mbReadData[ConfigSlaves[i].nRegs];
      //
      i++;
      }
  ///////
  //**REGISTROS
 
  i=0;
  j=0;
 for (pugi::xml_node comm = doc.child("comm"); comm; comm = comm.next_sibling("comm"))
    {
      i=0;
      //checking type
      for(int h=0;h<ntypes;h++)
	{
	  //cout << "DEBUG: comparando entre: " << comm.attribute("Tregister").value() << " y " << stypes[h] << endl;
	  if(!strcmp(comm.attribute("Tregister").value(),stypes[h]))
	    ConfigSlaves[j].Registers[i].Datatype = itypes[h];
	}
      ConfigSlaves[j].Registers[i].address = comm.attribute("Aregister").value();	 
      for(i = 1; i <  ConfigSlaves[j].nRegs;i++)
	{
	  //cout << "slave: " << j << "register; " << i << endl;
	  //cout << "numero de registros: " <<  ConfigSlaves[j].nRegs << endl;
	  //cout << "EN EL FOR!!" << endl;
	  sprintf(buffer,"Tregister%d",i+1);
	  //cout << " DEBUG: " << comm.attribute(buffer).value() << endl;
	  //checking type
	  for(int h=0;h<ntypes;h++)
	    {
	      //cout << "DEBUG: comparando entre: " << comm.attribute(buffer).value() << " y " << stypes[h] << endl;
	      if(!strcmp(comm.attribute(buffer).value(),stypes[h]))
		ConfigSlaves[j].Registers[i].Datatype = itypes[h];
	    }
	  sprintf(buffer,"Aregister%d",i+1);
	  //cout << " DEBUG: " << comm.attribute(buffer).value() << endl;
	  ConfigSlaves[j].Registers[i].address = comm.attribute(buffer).value();

	}
      j++;
      }
  return 0;
}
*/
/*!debug show config information*//*
int configParser::RetrieveConfig()
{
  int i=0,j=0;
  cout << "number of slaves configured: " << nSlaves << endl;

  for (i=0;i<nSlaves;i++)
    {
      cout << "************************************" << endl;
      cout << "*** SLAVE "<< i+1 <<" ***" << endl;
      cout << endl;
   
      cout << " ID = " << ConfigSlaves[i].slaveId << endl;
      cout << " mTime = " << ConfigSlaves[i].mTime << endl;
      cout << " registers = " << ConfigSlaves[i].nRegs << endl;

      cout << "** REGISTROS **" << endl;
      
      for(j=0;j<ConfigSlaves[i].nRegs;j++)
	{
	   cout << " numero = " << j+1 << endl;
	   cout << " tipo = " << ConfigSlaves[i].Registers[j].Datatype << endl;
	   cout << " direccion = " << ConfigSlaves[i].Registers[j].address << endl;
	   cout << endl;
	   }
	 
    }

  return 0;
  }*/
/*
int configParser::getSlaveId(int id)
{
  if(id >=0 && id <nSlaves)
    return ConfigSlaves[id].slaveId;
  else
    return -1;
}
*/
/*
int configParser::getSlaveMailbox(int id,rlDataAcquisition **mailBox)
{
  if(id >=0 && id <nSlaves)
    *mailBox = ConfigSlaves[id].Mailbox;
  else
    return -1;
  return 0;
}
*/
/*
int configParser::getSlavemTime(int id)
{  
  if(id >=0 && id <nSlaves)
    return ConfigSlaves[id].mTime;
  else
    return -1;

}
*/
/*
int configParser::getSlavenRegs(int id)
{
  if(id >=0 && id <nSlaves)
    return ConfigSlaves[id].nRegs;
  else
    return -1;

}
*/
/*
int configParser::getSlaveReadData(int id,mbReadData **rData)
{
  if(id >=0 && id <nSlaves)
    *rData = ConfigSlaves[id].Registers;
  else
    return -1;
  return 0;
}
*/
