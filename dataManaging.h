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
@File capture.h
'''personalize this files for custom data structures'''
*/
#ifndef _DATA_
#define _DATA_


#include <ctime>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "rldataacquisition.h"
#include "libgStools.h"

/*!register to save*/
typedef struct 
{
  char address[35];
  char addressL[30];
  char addressH[30];
  char addressHH[30];
  int iaddress;
  int islave;
  char tag_name[20];
  int valueLL;
  int valueL;
  int valueH;
  int valueHH;
  char date[20];
  char time[20];
  int cycle;
  char comment[100];
  int type;
  int err;
  int trigger;
} mbRegData;


class table
{
 public:
  table(int kind);
  ~table();

  //custom
  int createTableRecepcion();
  int createTableProceso();
  int createTableProducto();

  //setTrigger();
  int checkTrigger();
  int resetTrigger();

  int takeValues();
  int takeRec();
  int takeLim();
  int takeBig();
  char* storeValues();
  char* storeJuliano();
  int setJuliano(int);

 private:
  
  mbRegData* regs;
  rlDataAcquisition *Mailbox;
  int internalKind;

  //custom
  //table values
  int xxxtipo;
  int xxxproveedor;
  int xxxsilo;
  char xxxmatricula[8];
  int xxxkilos;
  int xxxvoid1;
  int xxxvoid2;
  int xxxvoid3;
  int xxxvoid4;
  int xxxvoid5;
  
  int yyytipo;
  int yyysiloO;
  int yyysiloD;
  
  int zzztipo;
  int zzzsiloO;
  int zzzkilos;
  int zzzjuliano;

};


#endif
