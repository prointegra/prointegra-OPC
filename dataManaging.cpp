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
@File capture.cpp
'''personalize this files for custom data structures'''
*/

#include "dataManaging.h"

/*! CONSTRUCTOR, kind of preset table to instance*/
table::table(int kind)
{
#ifdef __FUCKIN_WINDOWS
  Mailbox = new   rlDataAcquisition("c:\\modbus.mbx","c:\\modbus.shm",128536);
#else
  Mailbox = new   rlDataAcquisition("../modbus.mbx","../modbus.shm",128536);
#endif
  internalKind = kind;
  if (kind==0)
    createTableRecepcion();
  else if(kind==1)
    createTableProceso();
  else
    createTableProducto();

  return;

}

/*! DESTRUCTOR*/
table::~table()
{
  delete regs;
  return;
}

  //custom
int table::createTableRecepcion()
{
  
  ///structure:
  //trigger / INT
  //tipo producto / INT
  //matricula vehiculo // 8 caracteres -> 4xINT?
  //proveedor/ INT
  //silo de recepcion / INT
  //vacio1 / INT
  //vacio2 / INT
  //vacio3 / INT
  //VACIO4 / INT
  //vacio5 / INT
  regs = new mbRegData[20];

  //trigger
  strcpy(regs[0].address,"holdingRegisters(1,1000)");
  regs[0].iaddress = 1000;
  regs[0].islave = 1;
  strcpy(regs[0].tag_name,"trigger");
  regs[0].trigger = 1;
  regs[0].type = 0; //INT
  //tipo de producto
  strcpy(regs[1].address,"holdingRegisters(1,1001)");
  strcpy(regs[1].tag_name,"tipo_prod");
  regs[1].trigger = 0;
  regs[1].type = 0; //INT
  //matricula
  strcpy(regs[2].address,"holdingRegisters(1,1002)");
  strcpy(regs[2].addressL,"holdingRegisters(1,1003)");
  strcpy(regs[2].addressH,"holdingRegisters(1,1004)");
  strcpy(regs[2].addressHH,"holdingRegisters(1,1005)");
  strcpy(regs[2].tag_name,"matricula");
  regs[2].trigger = 0;
  regs[2].type = 1; //CADENAx8
  //proveedor
  strcpy(regs[3].address,"holdingRegisters(1,1006)");
  strcpy(regs[3].tag_name,"proveedor");
  regs[3].trigger = 0;
  regs[3].type = 0; //INT
  //silo_recepcion
  strcpy(regs[4].address,"holdingRegisters(1,1007)");
  strcpy(regs[4].tag_name,"silo");
  regs[4].trigger = 0;
  regs[4].type = 0; //INT
  //kilos
  strcpy(regs[5].address,"holdingRegisters(1,1008)");
  strcpy(regs[5].tag_name,"kilos");
  regs[5].trigger = 0;
  regs[5].type = 0; //INT
  //vacio 1
  strcpy(regs[6].address,"holdingRegisters(1,1009)");
  strcpy(regs[6].tag_name,"void1");
  regs[6].trigger = 0;
  regs[6].type = 0; //INT  
  //vacio 2
  strcpy(regs[7].address,"holdingRegisters(1,1010)");
  strcpy(regs[7].tag_name,"void1");
  regs[7].trigger = 0;
  regs[7].type = 0; //INT
  //vacio 3
  strcpy(regs[8].address,"holdingRegisters(1,1011)");
  strcpy(regs[8].tag_name,"void1");
  regs[8].trigger = 0;
  regs[8].type = 0; //INT
  //vacio 4
  strcpy(regs[9].address,"holdingRegisters(1,1012)");
  strcpy(regs[9].tag_name,"void1");
  regs[9].trigger = 0;
  regs[9].type = 0; //INT
  //vacio 5
  strcpy(regs[10].address,"holdingRegisters(1,1013)");
  strcpy(regs[10].tag_name,"void1");
  regs[10].trigger = 0;
  regs[10].type = 0; //INT   

  return 0;

}

int table::createTableProceso()
{
  ///structure:
  //trigger / INT
  //tipo producto / INT
  //silo origen / INT
  //silo destino / INT
  regs = new mbRegData[6];
  //trigger
  strcpy(regs[0].address,"holdingRegisters(1,1020)");
  regs[0].iaddress = 1020;
  regs[0].islave = 1;
  strcpy(regs[0].tag_name,"trigger");
  regs[0].trigger = 1;
  regs[0].type = 0; //INT
  //tipo de producto
  strcpy(regs[1].address,"holdingRegisters(1,1021)");
  strcpy(regs[1].tag_name,"tipo_prod");
  regs[1].trigger = 0;
  regs[1].type = 0; //INT
  //silo_recepcion
  strcpy(regs[2].address,"holdingRegisters(1,1022)");
  strcpy(regs[2].tag_name,"silo_origen");
  regs[2].trigger = 0;
  regs[2].type = 0; //INT
  //silo_limpia
  strcpy(regs[3].address,"holdingRegisters(1,1023)");
  strcpy(regs[3].tag_name,"silo_destino");
  regs[3].trigger = 0;
  regs[3].type = 0; //INT

  return 0;
}

int table::createTableProducto()
{
  ///structure:
  //trigger / INT
  //tipo producto / INT
  //silo origen / INT
  //Kilos / INT
  regs = new mbRegData[6]; 
  //trigger
  strcpy(regs[0].address,"holdingRegisters(1,1040)");
  regs[0].iaddress = 1040;
  regs[0].islave = 1;
  strcpy(regs[0].tag_name,"trigger");
  regs[0].trigger = 1;
  regs[0].type = 0; //INT
  //tipo de producto
  strcpy(regs[1].address,"holdingRegisters(1,1041)");
  strcpy(regs[1].tag_name,"tipo_prod");
  regs[1].trigger = 0;
  regs[1].type = 0; //INT
  //silo_limpia
  strcpy(regs[2].address,"holdingRegisters(1,1042)");
  strcpy(regs[2].tag_name,"silo_origen");
  regs[2].trigger = 0;
  regs[2].type = 0; //INT
  //kilos envasados
  strcpy(regs[3].address,"holdingRegisters(1,1043)");
  strcpy(regs[3].tag_name,"kilos");
  regs[3].trigger = 0;
  regs[3].type = 0; //INT

  
  return 0;
}
/*! first value is the trigger by default */
int table::checkTrigger()
{

  //recepcion trigger
  if (Mailbox->intValue(regs[0].address)==0 || Mailbox->intValue(regs[0].address)==50)
    {
      return 0;
    }
  else
    {
      if (Mailbox->intValue(regs[0].address) == (256*256*128)) //COMM ERROR
		    return 0;
    }
  //std::cout << "DEBUG: trigger table value = " << Mailbox->intValue(regs[0].address) << ",  and address: " <<regs[0].address<<  std::endl;
    return 1;

}
/*! first value is the trigger by default */
int table::resetTrigger()
{
  char text[40];
  sprintf(text,"register(%d,%d)",regs[0].islave,regs[0].iaddress);
  //std::cout << text << std::endl;
  //value 50 to reset trigger
  Mailbox->writeIntValue(text,50); 
  return 0;
    
}

int table::takeRec()
{
      xxxtipo = Mailbox->intValue(regs[1].address);
      xxxproveedor = Mailbox->intValue(regs[3].address);
      xxxsilo = Mailbox->intValue(regs[4].address);
      xxxkilos = Mailbox->intValue(regs[5].address);


      //std::cout << "VARIABLES! " << std::endl;
      //std::cout << "tipo: " << xxxtipo << std::endl;
      //std::cout << "proveedor: " << xxxproveedor << std::endl;
      //std::cout << "silo: " << xxxsilo << std::endl;
      //std::cout << "kilos: " << xxxkilos << std::endl;
      //std::cout << "matricula: " << xxxmatricula << std::endl;
      //we asume first byte low communicationsstorage in PLC
      memset(xxxmatricula, 0, sizeof(xxxmatricula));
      xxxmatricula[0] =  (char)Mailbox->intValue(regs[2].address);
      xxxmatricula[1] = (char)(Mailbox->intValue(regs[2].address) >> 8);
      xxxmatricula[2] =  (char)Mailbox->intValue(regs[2].addressL);
      xxxmatricula[3] = (char)(Mailbox->intValue(regs[2].addressL) >> 8);
      xxxmatricula[4] =  (char)Mailbox->intValue(regs[2].addressH);
      xxxmatricula[5] = (char)(Mailbox->intValue(regs[2].addressH) >> 8);
      xxxmatricula[6] =  (char)Mailbox->intValue(regs[2].addressHH);
      xxxmatricula[7] = (char)(Mailbox->intValue(regs[2].addressHH) >> 8);
      //xxxmatricula[8] = '\0';
      //std::cout << "matricula: " << xxxmatricula << std::endl;

      xxxvoid1 = Mailbox->intValue(regs[6].address);
      xxxvoid2 = Mailbox->intValue(regs[7].address);
      xxxvoid3 = Mailbox->intValue(regs[8].address);      
      xxxvoid4 = Mailbox->intValue(regs[9].address);
      xxxvoid5 = Mailbox->intValue(regs[10].address);

      return 0;
}
int table::takeLim()
{
  yyytipo = Mailbox->intValue(regs[1].address);
  yyysiloO = Mailbox->intValue(regs[2].address);
  yyysiloD = Mailbox->intValue(regs[3].address);

  return 0;
}
int table::takeBig()
{
  zzztipo = Mailbox->intValue(regs[1].address);
  zzzsiloO = Mailbox->intValue(regs[2].address);
  zzzkilos = Mailbox->intValue(regs[3].address);

  return 0;
}


int table::takeValues()
{
  if(internalKind==0)
    takeRec();
  else if (internalKind==1)
    takeLim();
  else if (internalKind==2)
    takeBig();
    
  return 0;
}
char* table::storeValues()
{
  char *sql = new char[500];
  time_t t = time(0);
  struct tm * now = localtime( & t );
  int juliano;

  if (internalKind == 0) //store recepcion
    {

      sprintf(sql,"INSERT INTO recepcion(FECHA,HORA,SILO_RECEPCION,TIPO_DE_PRODUCTO,KILOS,MATRICULA,PROVEEDOR,VACIO1,VACIO2,VACIO3,VACIO4,VACIO5) VALUES(\'%d/%d/%d\',\'%d:%d:%d\',%d,%d,%d,\'%s\',%d,%d,%d,%d,%d,%d)",(now->tm_year + 1900),(now->tm_mon + 1),now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec,xxxsilo ,xxxtipo, xxxkilos, xxxmatricula, xxxproveedor, xxxvoid1,xxxvoid2,xxxvoid3,xxxvoid4,xxxvoid5);
      //std::cout << sql << std::endl;
      return (sql);
    }
  else if (internalKind == 1) //store limpia
    {

      sprintf(sql,"INSERT INTO proceso(FECHA,HORA,SILO_RECEPCION,SILO_LIMPIA,TIPO_DE_PRODUCTO) VALUES(\'%d/%d/%d\',\'%d:%d:%d\',%d,%d,%d)",(now->tm_year + 1900),(now->tm_mon + 1),now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec, yyysiloO, yyysiloD, yyytipo);
      //std::cout << sql << std::endl;
      return (sql);
    }
  else if (internalKind == 2) //store producto
    {
      juliano = gstTime2JulianI(&now);
      sprintf(sql,"INSERT INTO producto(FECHA,HORA,JULIANO_ENVASADO,JULIANO_LIMPIA,TIPO_DE_PRODUCTO,SILO_LIMPIA,KILOS) VALUES(\'%d/%d/%d\',\'%d:%d:%d\',%d,%d,%d,%d,%d)",(now->tm_year + 1900),(now->tm_mon + 1),now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec,juliano ,zzzjuliano, zzztipo, zzzsiloO, zzzkilos);
      //std::cout << sql << std::endl;
      return (sql);
    }
  return "NULL";
}

char* table::storeJuliano()
{
  char *sql = new char[500];
  time_t     now;
  int juliano;
  now = time(0);
  struct tm *ts;
  ts = localtime(&now);

  juliano = gstTime2JulianI(&ts);
  if (internalKind == 1) //store limpia
    {

      sprintf(sql,"UPDATE julianos set juliano = %d WHERE silo LIKE %d",juliano,yyysiloD);
      return (sql);
    }
  return ("NULL");
}
/*!storing julian date*/
int table::setJuliano(int juliano)
{

  zzzjuliano = juliano;
  return zzzjuliano;
}
